/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014, 2015   Michel Megens <dev@michelmegens.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file kernel/sched/core.c Scheduling core
 *
 * The scheduling core is the backend of the threading API. It provides
 * architecture independent code (algorithms, backend functions, etc..) to
 * support the threading API in dividing CPU time as fair as possible.
 */

/**
 * @addtogroup sched
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/compiler.h>
#include <etaos/types.h>
#include <etaos/init.h>
#include <etaos/list.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/preempt.h>
#include <etaos/irq.h>
#include <etaos/irq_handle.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/spinlock.h>

#include <asm/io.h>
#include <asm/sched.h>

static void raw_rq_add_thread(struct rq *rq, struct thread *tp);
static int raw_rq_remove_thread(struct rq *rq, struct thread *tp);
static bool thread_is_idle(struct thread *tp);

#ifdef CONFIG_IRQ_THREAD
DEFINE_THREAD_QUEUE(irq_thread_queue);

/**
 * @brief Put an IRQ thread in a waiting state.
 * @note The thread wont wake up until it is signaled and woken up by
 *       irq_signal_threads.
 * @see irq_signal_threads schedule
 */
static void irq_thread_wait(void)
{
	struct thread *tp = current_thread();
	
	set_bit(THREAD_WAITING_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
	schedule();
}

/**
 * @brief Threaded IRQ handle.
 * @param data IRQ data.
 * @note This function is a thread handle.
 *
 * Threaded IRQ's will be handled in this thread. Calls 
 * struct irq_data::handler.
 */
void irq_handle_fn(void *data)
{
	struct irq_data *irq = data;

	while(true) {
		irq_thread_wait();
		if(test_bit(THREAD_EXIT_FLAG, &current_thread()->flags))
			kill();

		if(irq->handler(irq, irq->private_data) == IRQ_HANDLED)
			clear_bit(IRQ_THREADED_TRIGGERED_FLAG, &irq->flags);
	}
}
#endif

#ifdef CONFIG_THREAD_QUEUE
/**
 * @brief Get the thread_queue the given thread is on.
 * @param tp Thread to get the thread_queue for.
 * @note You have to be sure that the thread is on one. If its not, a false
 *       thread queue is returned.
 */
static inline struct thread_queue *thread_to_queue(struct thread *tp)
{
	struct thread **tpp = (struct thread**)tp->queue;

	if(tpp)
		return container_of(tpp, struct thread_queue, qhead);
	else
		return NULL;
}

static void queue_wait_tmo(struct timer *timer, void *arg)
{
	struct thread *tp = arg;
	struct thread_queue *qp = thread_to_queue(tp);

	queue_remove_thread(qp, tp);
	if(qp->qhead == NULL)
		qp->qhead = SIGNALED;

	rq_add_thread_no_lock(tp);
}

void thread_queue_wait(struct thread_queue *qp, unsigned int ms)
{
	struct thread *current = current_thread();

	current->timer = tm_create_timer(current->rq->source, ms,
			&queue_wait_tmo, current, TIMER_ONESHOT_MASK);
	queue_add_thread(qp, current);
	schedule();
}

/**
 * @brief Initialise a thread queue during run time.
 * @param qp Thread queue which has to be initialised.
 */
void thread_queue_init(struct thread_queue *qp)
{
	qp->sched_class = &fifo_class;
	qp->qhead = SIGNALED;
	spinlock_init(&qp->lock);
}

/**
 * @brief Add a new thread to a queue.
 * @param qp Queue to add to.
 * @param tp Thread to add.
 */
static void raw_queue_add_thread(struct thread_queue *qp,
					struct thread *tp)
{
	struct sched_class *cp;
	unsigned long flags;

	if(tp->on_rq && test_bit(THREAD_RUNNING_FLAG, &tp->flags)) {
		raw_spin_lock_irqsave(&tp->rq->lock, flags);
		raw_rq_remove_thread_noresched(tp->rq, tp);
		raw_spin_unlock_irqrestore(&tp->rq->lock, flags);
	}


	cp = qp->sched_class;
	if(cp)
		cp->queue_add(qp, tp);
}

/**
 * @brief Remove a thread from a thread_queue.
 * @param qp Queue to remove from.
 * @param tp Thread which has to be removed.
 * @note No queue locks are aquired.
 */
static void raw_queue_remove_thread(struct thread_queue *qp,
						struct thread *tp)
{
	struct sched_class *cp;

	cp = qp->sched_class;
	if(cp)
		cp->queue_rm(qp, tp);
}

/**
 * @brief Add a new thread to a queue.
 * @param qp Queue to add to.
 * @param tp Thread to add.
 */
void queue_add_thread(struct thread_queue *qp, struct thread *tp)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&qp->lock, flags);
	raw_queue_add_thread(qp, tp);
	raw_spin_unlock_irqrestore(&qp->lock, flags);
}

/**
 * @brief Remove a thread from a thread_queue.
 * @param qp Queue to remove from.
 * @param tp Thread which has to be removed.
 */
void queue_remove_thread(struct thread_queue *qp, struct thread *tp)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&qp->lock, flags);
	raw_queue_remove_thread(qp, tp);
	raw_spin_unlock_irqrestore(&qp->lock, flags);
}
#endif

/**
 * @brief Remove a thread from the rq_list.
 * @param tp Thread to remove.
 * @param tpp Root queue pointer.
 * @return Error code.
 * @retval -EOK on success.
 */
static int rq_list_remove(struct thread *tp, struct thread *volatile*tpp)
{
	struct thread *carriage;

	/* get the queue head */
	carriage = *tpp;
	if(carriage == SIGNALED)
		return -EINVAL;

	/* loop through all threads in the queue */
	while(carriage) {
		if(carriage == tp) {
			*tpp = tp->rq_next;
			tp->rq_next = NULL;
			tp->queue = NULL;
			break;
		}

		tpp = &carriage->rq_next;
		carriage = carriage->rq_next;
		continue;
	}

	return -EOK;
}

/**
 * @brief Remove a thread from a rq.
 * @param rq RQ to remove from.
 * @param tp Thread to remove.
 * @note No locks are aquired.
 */
void raw_rq_remove_wake_thread(struct rq *rq, struct thread *tp)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	rq_list_remove(tp, &rq->wake_queue);
	irq_restore(&flags);
}

/**
 * @brief Remove a thread from the kill queue.
 * @param rq RQ to remove from.
 * @param tp Thread to remove.
 */
void raw_rq_remove_kill_thread(struct rq *rq, struct thread *tp)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	rq_list_remove(tp, &rq->kill_queue);
	irq_restore(&flags);
}

/**
 * @brief Remove a thread from a rq.
 * @param rq RQ to remove from.
 * @param tp Thread to remove.
 *
 * struct rq::lock will be locked (and unlocked).
 */
void rq_remove_wake_thread(struct rq *rq, struct thread *tp)
{
	raw_spin_lock(&rq->lock);
	raw_rq_remove_wake_thread(rq, tp);
	raw_spin_unlock(&rq->lock);
}

/**
 * @brief Remove a thread from the kill queue.
 * @param rq RQ to remove from.
 * @param tp Thread to remove.
 *
 * struct rq::lock will be locked (and unlocked).
 */
void rq_remove_kill_thread(struct rq *rq, struct thread *tp)
{
	raw_spin_lock(&rq->lock);
	raw_rq_remove_kill_thread(rq, tp);
	raw_spin_unlock(&rq->lock);
}

/**
 * @brief Add a thread to rq_list.
 * @param new Thread to add.
 * @param head New is add after this thread
 */
static inline void rq_list_add(struct thread *new, struct thread *head)
{
	new->rq_next = head;
}

/**
 * @brief Add a wake thread.
 * @param rq RQ to add to.
 * @param new Thread to add.
 */
static void rq_add_wake_thread(struct rq *rq, struct thread *new)
{
	new->rq = rq;
	rq_list_add(new, rq->wake_queue);
	rq->wake_queue = new;
}

/**
 * @brief Add a kill thread.
 * @param rq RQ to add to.
 * @param new Thread to add.
 */
static void rq_add_kill_thread(struct rq *rq, struct thread *new)
{
	new->rq = rq;
	rq_list_add(new, rq->kill_queue);
	rq->kill_queue = new;
}

/**
 * @brief Add a wake thread to the current RQ.
 * @param tp Thread to add.
 */
void raw_thread_add_to_wake_q(struct thread *tp)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	rq_add_wake_thread(rq, tp);
}

/**
 * @brief Add a kill thread to the current RQ.
 * @param tp Thread to add.
 */
void raw_thread_add_to_kill_q(struct thread *tp)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	rq_add_kill_thread(rq, tp);
}

/**
 * @brief Add a wake thread to the current RQ.
 * @param tp Thread to add.
 *
 * struct rq::lock will be locked (and unlocked).
 */
void thread_add_to_wake_q(struct thread *tp)
{
	struct rq *rq;
	unsigned long flags;

	rq = sched_get_cpu_rq();
	raw_spin_lock_irqsave(&rq->lock, flags);
	raw_thread_add_to_wake_q(tp);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

/**
 * @brief Add a kill thread to the current RQ.
 * @param tp Thread to add.
 *
 * struct rq::lock will be locked (and unlocked).
 */
void thread_add_to_kill_q(struct thread *tp)
{
	struct rq *rq;
	unsigned long flags;

	rq = sched_get_cpu_rq();
	raw_spin_lock_irqsave(&rq->lock, flags);
	raw_thread_add_to_kill_q(tp);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

/**
 * @brief Add a thread to a run queue.
 * @param rq Run queue to add \p tp to.
 * @param tp Thread to add.
 */
static void raw_rq_add_thread(struct rq *rq, struct thread *tp)
{
	struct sched_class *class = rq->sched_class;

	class->add_thread(rq, tp);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	tp->on_rq = true;
	tp->rq = rq;

	return;
}

/**
 * @brief Add a thread to a run queue.
 * @param tp Thread to add.
 * @note No locks are aquired.
 */
void rq_add_thread_no_lock(struct thread *tp)
{
	struct rq *rq;
	struct sched_class *class;

	rq = sched_get_cpu_rq();
	class = rq->sched_class;
	class->add_thread(rq, tp);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	tp->on_rq = true;
	tp->rq = rq;

	return;
}

/**
 * @brief Add a thread to a run queue.
 * @param rq Run queue to add \p tp to.
 * @param tp Thread to add.
 *
 * struct rq::lock will be locked (and unlocked).
 */
int rq_add_thread(struct rq *rq, struct thread *tp)
{
	int err;
	struct prev_rq;
	unsigned long flags;
	struct sched_class *class = rq->sched_class;

	err = 0;
	if(tp->on_rq && test_bit(THREAD_RUNNING_FLAG, &tp->flags))
		err = rq_remove_thread(tp);

	raw_spin_lock_irqsave(&rq->lock, flags);
	class->add_thread(rq, tp);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
	tp->on_rq = true;
	tp->rq = rq;

	return err;
}

/**
 * @brief Remove a thread from a run queue without rescheduling.
 * @param rq RQ to remove from.
 * @param tp Thread to remove.
 */
int raw_rq_remove_thread_noresched(struct rq *rq, struct thread *tp)
{
	struct sched_class *class;
	int err;

	if(!tp->on_rq) {
		err = -EINVAL;
	} else if(rq->current == tp) {
		/*
		 * The current thread is to be
		 * removed from the RQ. This requires
		 * it to be stopped first.
		 */
		clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
		set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
		err = 1;
	} else {
		class = rq->sched_class;
		clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
		err = class->rm_thread(rq, tp);
		tp->on_rq = false;
	}

	return err;
}

/**
 * @brief Remove a thread from a run queue.
 * @param rq RQ to add to.
 * @param tp Thread to remove.
 */
static int raw_rq_remove_thread(struct rq *rq, struct thread *tp)
{
	int err;

	err = raw_rq_remove_thread_noresched(rq, tp);
	
	if(err > 0) {
		schedule();
		err = 0;
	}

	return err;
}

/**
 * @brief Remove a thread from a run queue.
 * @param tp Thread to remove.
 *
 * struct rq::lock will be locked (and unlocked).
 */
int rq_remove_thread(struct thread *tp)
{
	int err;
	struct rq *rq;
	unsigned long flags;

	rq = tp->rq;
	if(rq->current != tp) {
		raw_spin_lock_irqsave(&rq->lock, flags);
		err = raw_rq_remove_thread(rq, tp);
		raw_spin_unlock_irqrestore(&rq->lock, flags);
	} else {
		err = raw_rq_remove_thread(rq, tp);
	}

	return err;
}

/**
 * @brief Update a run queue.
 * @param rq Run queue to update.
 * @note Calls sched_class::post_schedule.
 */
static void rq_update(struct rq *rq)
{
	struct sched_class *class;

	class = rq->sched_class;
	if(class->post_schedule)
		class->post_schedule(rq);
}

/**
 * @brief Timer handle for sleeping threads.
 * @param timer Timer pointer.
 * @param arg Timer argument, which points to the sleeping thread.
 */
static void sched_sleep_timeout(struct timer *timer, void *arg)
{
	struct thread *tp;
	struct rq *rq;

	tp = arg;
	rq = sched_get_cpu_rq();

	clear_bit(THREAD_SLEEPING_FLAG, &tp->flags);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	raw_rq_add_thread(rq, tp);
}

/**
 * @brief Prepare a thread to go into sleeping mode.
 * @param tp Thread to prepare.
 * @param ms Time to sleep in miliseconds.
 */
void sched_setup_sleep_thread(struct thread *tp, unsigned ms)
{
	struct rq *rq;

	preempt_disable();
	rq = tp->rq;
	set_bit(THREAD_SLEEPING_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);

	tp->timer = tm_create_timer(rq->source, ms, &sched_sleep_timeout,
			tp, TIMER_ONESHOT_MASK);
	preempt_enable_no_resched();
}

/**
 * @brief Obtain the next runnable task of a run queue.
 * @param rq Run queue to get the next runnable from.
 */
static struct thread *sched_get_next_runnable(struct rq *rq)
{
	struct thread *next;
	struct sched_class *class = rq->sched_class;

	if(class) {
		next = class->next_runnable(rq);
		if(!next)
			next = rq->current;

		return next;
	} else {
		return rq->current;
	}
}

/**
 * @brief Destroy the kill queue.
 * @param rq Run queue containing the kill queue head.
 */
static void rq_destroy_kill_q(struct rq *rq)
{
	struct thread *walker, *tmp;

	walker = rq->kill_queue;
	if(!walker)
		return;

	raw_spin_lock_irq(&rq->lock);
	for(tmp = walker->rq_next; walker; 
			walker = tmp, tmp = walker->rq_next) {
		raw_rq_remove_kill_thread(rq, walker);
		sched_free_stack_frame(walker);
		kfree(walker);
	}
	raw_spin_unlock_irq(&rq->lock);
}

#ifdef CONFIG_DYN_PRIO
/**
 * @brief Reset the dynamic priority.
 * @param tp Thread to reset the dynamic priority for.
 */
static inline void dyn_prio_reset(struct thread *tp)
{
	tp->dprio = 0;
}

/**
 * @brief Update the dynamic priorities of a run queue with 1.
 * @param rq Run queue to update.
 */
static inline void dyn_prio_update(struct rq *rq)
{
	struct sched_class *class;

	class = rq ? rq->sched_class : NULL;
	if(likely(class))
		class->dyn_prio_update(rq, 1);
}
#else
static inline void dyn_prio_reset(struct thread *tp)
{
}

static inline void dyn_prio_update(struct rq *rq)
{
}
#endif

/**
 * @brief Switch context on a given run queue.
 * @param rq Run queue to use for the context.
 * @param prev Previous thread.
 * @param new New thread to replace \p prev.
 */
static void __hot rq_switch_context(struct rq *rq, struct thread *prev,
						struct thread *new)
{
	struct sched_class *class = rq->sched_class;

	if(prev) {
		if(test_bit(THREAD_RUNNING_FLAG, &prev->flags)) {
			prev->rq = rq;
			prev->on_rq = true;
			class->add_thread(rq, prev);
		} else {
			prev->rq = NULL;
			prev->on_rq = false;
		}
	}

	/* prev != new (this condition is ensured by __schedule) */
	class->rm_thread(rq, new);
	raw_spin_unlock_irq(&rq->lock);
	cpu_switch_context(rq, prev, new);
}

#ifdef CONFIG_EVENT_MUTEX
/**
 * @brief Signal a thread on a wake queue.
 * @param rq Run queue of the thread.
 * @param tp Thread to signal.
 *
 * The thread \p tp has received an EVENT trigger. The queue which \p tp is on
 * will be notified and the the thread with the highest priority will be woken
 * up first.
 */
static void rq_signal_event_queue(struct rq *rq, struct thread *tp)
{
	struct thread_queue *qp;
	unsigned long flags;

	irq_save_and_disable(&flags);
	qp = thread_to_queue(tp);
	tp = *tp->queue;

	raw_rq_remove_wake_thread(rq, tp);
	raw_queue_remove_thread(qp, tp);

	if(!qp->qhead)
		qp->qhead = SIGNALED;

	if(tp->timer && tp->timer != SIGNALED) {
		tm_stop_timer(tp->timer);
	}

	if(unlikely(rq->current != tp)) {
		raw_rq_add_thread(rq, tp);
		if(prio(tp) <= prio(rq->current))
			set_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags);
	} else {
		set_bit(THREAD_RUNNING_FLAG, &tp->flags);
		tp->on_rq = true;
		tp->rq = rq;
	}
	irq_restore(&flags);
}
#endif

/**
 * @def current
 * @param _rq Run queue to get the current thread from.
 */
#define current(_rq) ((_rq)->current)

#ifdef CONFIG_IRQ_THREAD
/**
 * @brief Signal threads that function as an IRQ.
 * @param rq Current run queue.
 *
 * IRQ's can be configured to be handled in threaded context instead of IRQ
 * context. This function is responsible for waking up the correct threads
 * when one of these IRQ's has been triggered by external hardware.
 */
static void irq_signal_threads(struct rq *rq)
{
	struct list_head *icarriage;
	struct irq_chip *chip = irq_get_chip();
	struct irq_data *idata;
	struct irq_thread_data *tdata;
	struct thread *tp;

	list_for_each(icarriage, &chip->irqs) {
		idata = list_entry(icarriage, struct irq_data, irq_list);
		if(likely(!test_bit(IRQ_THREADED_FLAG, &idata->flags) ||
				!test_bit(IRQ_ENABLE_FLAG, &idata->flags)))
			continue;

		tdata = container_of(idata, struct irq_thread_data, idata);
		tp = tdata->owner;
		if(tp->ec) {
			clear_bit(THREAD_WAITING_FLAG, &tp->flags);
			rq_add_thread_no_lock(tp);
			set_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags);
		}
	}

	return;
}
#else
static inline void irq_signal_threads(struct rq *rq)
{
}
#endif

#ifdef CONFIG_EVENT_MUTEX
/**
 * @brief Signal threads that are waiting for an event.
 * @param rq A pointer to the current of 'this' CPU.
 *
 * This function wakes up all threads that have received an EVENT trigger. Only
 * functions that are put into a waiting state by the EVM subsystem are woken
 * up by this function. Functions put to sleep by either sleep or wait are not
 * affected.
 */
static void rq_signal_threads(struct rq *rq)
{
	struct thread *carriage, 
		      *tp,
		      *volatile*tpp;
	unsigned char events;

	carriage = rq->wake_queue;
	while(carriage) {
		events = carriage->ec;

		if(events) {
			tpp = carriage->queue;
			carriage->ec--;
			tp = *tpp;
			if(tp != SIGNALED)
				rq_signal_event_queue(rq, tp);
		}
		carriage = carriage->rq_next;
	}

	return;
}
#else
static inline void sched_do_signal_threads(struct rq *rq)
{
}
#endif

#ifdef CONFIG_PREEMPT
static inline void preempt_reset_slice(struct thread *tp)
{
	if(tp)
		tp->slice = CONFIG_TIME_SLICE;
}
#else
static inline void preempt_reset_slice(struct thread *tp)
{
}
#endif

/**
 * @brief Reschedule policy.
 * @param curr Current thread.
 * @param next Thread which is supposed to be running after \p curr.
 * @return an integer indicating whether to reschedule or not.
 * @retval 1 if __schedule should reschedule.
 * @retval 0 if __schedule should not reschedule.
 *
 * The decision to reschedule is made as following:
 *
 * Check the PREEMPT_NEED_RESCHED_FLAG flag. If this flag is set and \p next
 * is the idle thread a reschedule will only occur if THREAD_NEED_RESCHED_FLAG
 * is set (PREEMPT_NEED_RESCHED_FLAG is discarded). If \p next is \b not the
 * idle thread, the return value will be:
 * \f$ x \lor y \f$ \n
 * where \p x is the value of PREEMPT_NEED_RESCHED_FLAG; \n
 * \p y is the value of THREAD_NEED_RESCHED_FLAG. \n
 * \n
 * When preemption is not enabled the value of THREAD_NEED_RESCHED_FLAG is
 * returned.
 */
static int __schedule_need_resched(struct thread *curr, struct thread *next)
{
#ifdef CONFIG_PREEMPT
	if(test_bit(PREEMPT_NEED_RESCHED_FLAG, &curr->flags)) {
		if(thread_is_idle(next)) {
			clear_bit(PREEMPT_NEED_RESCHED_FLAG, &curr->flags);
			preempt_reset_slice(curr);
			return test_and_clear_bit(THREAD_NEED_RESCHED_FLAG, 
					&curr->flags);
		} else {
			return (test_and_clear_bit(PREEMPT_NEED_RESCHED_FLAG,
					&curr->flags) ||
				test_and_clear_bit(THREAD_NEED_RESCHED_FLAG,
						&curr->flags));
		}
	} else {
		return test_and_clear_bit(THREAD_NEED_RESCHED_FLAG, 
				&curr->flags);
	}
	
#else
	return test_and_clear_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags);
#endif
}

/**
 * @brief Reschedule the current run queue.
 * @param cpu ID of the CPU which should be rescheduled.
 * @note This function also updates:
 * 	   - threads signaled from an IRQ;
 * 	   - timers;
 * 	   - threads which have used up their time slice;
 * 	   - the kill queue of the run queue;
 * 	   - irq threads which have to be woken up.
 * @see __schedule_need_resched
 *
 * struct rq::lock will be locked (and unlocked).
 */
static void __hot __schedule(int cpu)
{
	struct rq *rq;
	struct thread *next,
		      *prev;
	tick_t tdelta = 0;

	preempt_disable();
	rq = cpu_to_rq(cpu);
	prev = rq->current;
	raw_spin_lock_irq(&rq->lock);

	irq_signal_threads(rq);
	rq_signal_threads(rq);

	tdelta = tm_update_source(rq->source);
	if(tdelta)
		tm_process_clock(rq->source, tdelta);

#ifdef CONFIG_PREEMPT
	if(prev->slice <= tdelta) {
		prev->slice = CONFIG_TIME_SLICE;
		set_bit(PREEMPT_NEED_RESCHED_FLAG, &prev->flags);
	} else {
		prev->slice -= tdelta;
	}
#endif

	next = sched_get_next_runnable(rq);

	/*
	 * Only reschedule if we have to. The decision is based on the
	 * THREAD_NEED_RESCHED_FLAG and the PREEMPT_NEED_RESCHED_FLAG. Also,
	 * if prev == next a reschedule is redundant.
	 */
	if(likely(__schedule_need_resched(prev, next) && prev != next)) {
		rq->current = next;
		rq->switch_count++;

		dyn_prio_update(rq);
		dyn_prio_reset(prev);
		dyn_prio_reset(next);

		preempt_reset_slice(next);
		preempt_reset_slice(prev);
		rq_switch_context(rq, prev, next);
		rq_update(rq);

	} else {
		raw_spin_unlock_irq(&rq->lock);
	}

	rq_destroy_kill_q(rq);
	preempt_enable_no_resched();
	return;
}

static inline bool need_resched()
{
	struct thread *curr = current_thread();

	return (test_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags) ||
		test_bit(PREEMPT_NEED_RESCHED_FLAG, &curr->flags));
}

/**
 * @brief Reschedule the current run queue.
 * @see yield
 * @see sleep
 * @see wait
 * @see signal
 *
 * Run the scheduler to allow other threads to access the CPU. Most likely
 * you don't want to call this function directly.
 */
void __hot schedule(void)
{
	int cpu;

	do {
		cpu = cpu_get_id();
		__schedule(cpu);
	} while(need_resched());
}

#ifdef CONFIG_PREEMPT
void __hot preempt_schedule(void)
{
	int cpu;

	/* we don't want to preempt the current process if either
	 * a) the interrupts are disabled or
	 * b) the preemption counter != 0
	 */
	if(likely(!preemptible()))
		return;

	do {
		cpu = cpu_get_id();
		__schedule(cpu);
	} while(need_resched());

	return;
}
#endif

static struct thread idle_thread, main_thread;
static uint8_t idle_stack[CONFIG_IDLE_STACK_SIZE];

THREAD(idle_thread_func, arg)
{
	struct thread *tp = arg;

	irq_enable();
	thread_initialise(&main_thread, "main", &main_thread_func, &main_thread,
			CONFIG_STACK_SIZE, main_stack_ptr, 120);
	preempt_disable();
	while(true) {
		set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
		schedule();
	}
}

/**
 * @brief Initialise the scheduler.
 * @note This function doesn't return.
 *
 * The arch init initialises the scheduler using this function during boot.
 */
void sched_init(void)
{
	struct rq *rq;

	if(sys_sched_class.init)
		sys_sched_class.init();

	sched_init_idle(&idle_thread, "idle", &idle_thread_func,
			&idle_thread, CONFIG_IDLE_STACK_SIZE, idle_stack);
	rq = sched_get_cpu_rq();
	idle_thread.rq = rq;
	rq->current = &idle_thread;

	cpu_switch_context(rq, NULL, &idle_thread);
}

#ifdef CONFIG_PREEMPT
static bool thread_is_idle(struct thread *tp)
{
	if(!tp)
		return false;
	
	return (tp == &idle_thread);
}

/**
 * @brief Get a pointer to the preemption counter of the current thread.
 * @return A pointer to the preemption counter of the current thread.
 *
 * Returns struct thread::preemt_cnt.
 */
int *preempt_counter_ptr(void)
{
	struct thread *tp = current_thread();
	return &tp->preemt_cnt;
}

/**
 * @brief Substract \p num from the preemption counter of the current thread.
 * @param num Amount to substract from the preemption counter.
 *
 * Substracts \p num from struct thread::preemt_cnt.
 */
void __preempt_add(int num)
{
	volatile int *preemt_ptr = preempt_counter_ptr();

	*preemt_ptr += num;
}

/**
 * @brief Add \p num to the preemption counter of the current thread.
 * @param num Amount to add to the preemption counter.
 *
 * Will add \p num to struct thread::preemt_cnt.
 */
void __preempt_sub(int num)
{
	volatile int *preemt_ptr = preempt_counter_ptr();

	*preemt_ptr -= num;
}
#endif

/**
 * @brief API interface for `yield()'.
 * @param rq Pointer to the active run queue.
 *
 * Yield the current thread to allow other threads access to the CPU.
 */
void sched_yield(struct rq *rq)
{
	struct thread *tp, *curr;
	struct sched_class *class;
	int cpu;

	class = rq->sched_class;
	tp = class->next_runnable(rq);
	curr = current(rq);
	cpu = cpu_get_id();

	if(test_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags)) {
		__schedule(cpu);
		return;
	} else if(tp != curr) {
		if(prio(tp) <= prio(curr)) {
			set_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags);
			__schedule(cpu);
		}
	}

	return;
}

#ifdef CONFIG_PREEMPT
/**
 * @brief Check if the current thread should be rescheduled.
 * @note The difference between should_resched and preempt_should_resched is
 * is that should_resched only checks if THREAD_NEED_RESCHED_FLAG is set,
 * preempt_should_resched also checks if the timeslice has expired.
 * @retval true if the system should reschedule.
 * @retval false if the system doesn't need to reschedule.
 */
bool preempt_should_resched(void)
{
	struct rq *rq;
	unsigned int diff;
	struct thread *tp = current_thread();

	rq = tp->rq;
	diff = tm_get_tick(rq->source);
	diff -= cs_last_update(rq->source);

	if(diff >= tp->slice) {
		set_bit(PREEMPT_NEED_RESCHED_FLAG, &tp->flags);
		return true;
	}

	return false;
}
#endif

/**
 * @brief Check if a reschedule is needed.
 * @return True or false based on whether a resched is needed or not.
 * @retval true if a resched is needed.
 * @retval false if no resched is needed.
 */
bool should_resched(void)
{
	struct thread *tp = current_thread();

	if(test_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags))
		return true;
	else
		return false;
}

#ifdef CONFIG_DYN_PRIO
/**
 * @brief Get the priority of a thread.
 * @param tp Thread to calculate the priority off.
 * @return The priority of \p tp.
 */
unsigned char prio(struct thread *tp)
{
	unsigned char retval;

	retval = tp->prio;
	if(tp->dprio <= retval)
		retval -= tp->dprio;
	else
		retval = 0;

	return retval;
}
#else
unsigned char prio(struct thread *tp)
{
	if(tp)
		return tp->prio;
	else
		return 0;
}
#endif

/* @} */

