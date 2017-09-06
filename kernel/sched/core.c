/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014, 2015, 2016, 2017   Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
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
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/event.h>
#include <etaos/power.h>
#include <etaos/cpu.h>
#include <etaos/preempt.h>
#include <etaos/irq.h>
#include <etaos/irq_handle.h>
#include <etaos/bitops.h>
#include <etaos/timer.h>
#include <etaos/spinlock.h>
#include <etaos/panic.h>
#include <etaos/tick.h>

#include <asm/io.h>
#include <asm/sched.h>

static void raw_rq_add_thread(struct rq *rq, struct thread *tp);
static int raw_rq_remove_thread(struct rq *rq, struct thread *tp);
#ifdef CONFIG_PREEMPT
static bool thread_is_idle(struct thread *tp);
#endif

#ifdef CONFIG_SQS
static DEFINE_RQ(grq, &sys_sched_class);

/**
 * @brief Get the global run queue
 * @return The global run queue.
 * @see sched_get_cpu_rq
 *
 * The most notable use of this function is in #sched_get_cpu_rq. This way
 * the scheduling core always gets the correct run queue independant of the
 * Kconfig configuration options.
 */
struct rq *sched_get_grq(void)
{
	return &grq;
}
#endif

/**
 * @brief Set the current thread for \p rq.
 * @param rq Run queue to set the running thread for.
 * @param tp Thread to set as current on \p rq.
 * @note This function assumes you hold struct rq::lock.
 */
static inline void __sched_set_current_thread(struct rq *rq, struct thread *tp)
{
	rq->current = tp;
}

/**
 * @brief Set the current thread for the runnign CPU.
 * @param tp Thread to set as current.
 * @note This function assumes you hold struct rq::lock.
 */
static void sched_set_current_thread(struct thread *tp)
{
	int cpu;
	struct rq *rq;

	cpu = cpu_get_id();
	rq = cpu_to_rq(cpu);
	__sched_set_current_thread(rq, tp);
}

/**
 * @brief Return the current thread for a given run queue.
 * @param rq Run queue to get the current thread for.
 * @return The current thread of \p rq.
 * @see current_thread
 */
static struct thread *current_thread_for(struct rq *rq)
{
	if(!rq)
		return NULL;

	return rq->current;
}

#ifdef CONFIG_IRQ_THREAD
/**
 * @brief Put an IRQ thread in a waiting state.
 * @note This is a specialised version of #wait.
 * @see signal irq_thread_signal wait
 */
static void irq_thread_wait(void)
{
	struct thread *tp;

	tp = current_thread();
	set_bit(THREAD_WAITING_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
	schedule();
}

/**
 * @brief Wake an IRQ thread up.
 * @param data Threaded IRQ to wake up.
 * @see signal wait irq_thread_wait
 * @note This function alters the run queue's.
 * @note This thread will attempt to preempt the currently running thread as
 *       early as possible by setting the `THREAD_NEED_RESCHED_FLAG`.
 */
void irq_thread_signal(struct irq_thread_data *data)
{
	struct thread *tp, *current;

	tp = data->owner;
	current = current_thread();

	if(tp->on_rq)
		return;

	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	clear_bit(THREAD_WAITING_FLAG, &tp->flags);
	rq_add_thread_no_lock(tp);

	set_bit(PREEMPT_NEED_RESCHED_FLAG, &current->flags);
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
		/* Only sleep if there are no events waiting */
		irq_thread_wait();

		if(test_bit(THREAD_EXIT_FLAG, &current_thread()->flags))
			kill();

		irq->handler(irq, irq->private_data);
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
 * @return The run queue \p tp is on.
 * @see container_of
 */
static inline struct thread_queue *thread_to_queue(struct thread *tp)
{
	struct thread **tpp = (struct thread**)tp->queue;

	if(tpp)
		return container_of(tpp, struct thread_queue, qhead);
	else
		return NULL;
}

/**
 * @brief Thread queue timeout function.
 * @param timer Timer that has timed out.
 * @param arg Thread related to the event.
 * @note This function is a timer handle.
 */
static void queue_wait_tmo(struct timer *timer, void *arg)
{
	struct thread *tp = arg;
	struct thread_queue *qp = thread_to_queue(tp);

	queue_remove_thread(qp, tp);
	if(qp->qhead == NULL)
		qp->qhead = SIGNALED;

	rq_add_thread_no_lock(tp);
}

/**
 * @brief Let the current thread wait on a thread queue.
 * @param qp Thread queue to wait on.
 * @param ms Miliseconds to wait on \p qp.
 *
 * The current thread is removed from the run queue and put on hold for \p ms
 * miliseconds.
 */
void thread_queue_wait(struct thread_queue *qp, unsigned int ms)
{
	struct thread *current = current_thread();

	current->timer = timer_create(current->rq->source, ms, &queue_wait_tmo,
			              current, TIMER_ONESHOT_MASK);
	queue_add_thread(qp, current);
	schedule();
}

/**
 * @brief Initialise a thread queue during run time.
 * @param qp Thread queue which has to be initialised.
 *
 * Initialise the thread queue to a signaled state.
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
void raw_queue_add_thread(struct thread_queue *qp, struct thread *tp)
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
void raw_queue_remove_thread(struct thread_queue *qp, struct thread *tp)
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
 * @return Error code.
 * @retval -EOK on success.
 */
static int rq_list_remove(struct thread *tp)
{
	list_del(&tp->entry);
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
	rq_list_remove(tp);
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
	rq_list_remove(tp);
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
	_raw_spin_lock(&rq->lock);
	raw_rq_remove_wake_thread(rq, tp);
	_raw_spin_unlock(&rq->lock);
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
	_raw_spin_lock(&rq->lock);
	raw_rq_remove_kill_thread(rq, tp);
	_raw_spin_unlock(&rq->lock);
}

/**
 * @brief Add a thread to rq_list.
 * @param lh List head to add \p tp to.
 * @param tp Thread to addd to \p lh.
 */
static inline void rq_list_add(struct list_head *lh, struct thread *tp)
{
	list_add(&tp->entry, lh);
}

/**
 * @brief Add a wake thread.
 * @param rq RQ to add to.
 * @param new Thread to add.
 */
static void rq_add_wake_thread(struct rq *rq, struct thread *new)
{
	new->rq = rq;
	rq_list_add(&rq->wake_head, new);
}

/**
 * @brief Add a kill thread.
 * @param rq RQ to add to.
 * @param new Thread to add.
 */
static void rq_add_kill_thread(struct rq *rq, struct thread *new)
{
	new->rq = rq;
	rq_list_add(&rq->kill_head, new);
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
}

/**
 * @brief Add a thread to a run queue.
 * @param tp Thread to add.
 * @note No locks are aquired.
 * @see raw_rq_add_thread
 */
void rq_add_thread_no_lock(struct thread *tp)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	raw_rq_add_thread(rq, tp);
}

/**
 * @brief Add a thread to a run queue.
 * @param rq Run queue to add \p tp to.
 * @param tp Thread to add.
 * @return An error code.
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
 * @return An error code.
 */
int raw_rq_remove_thread_noresched(struct rq *rq, struct thread *tp)
{
	struct sched_class *class;
	int err;

	if(!tp->on_rq) {
		err = -EINVAL;
	} else if(current_thread() == tp) {
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
 * @return An error code.
 */
static int raw_rq_remove_thread(struct rq *rq, struct thread *tp)
{
	int err;

	err = raw_rq_remove_thread_noresched(rq, tp);

	if(err > 0) {
		schedule();
		err = -EOK;
	}

	return err;
}

/**
 * @brief Remove a thread from a run queue.
 * @param tp Thread to remove.
 * @return An error code.
 *
 * struct rq::lock will be locked (and unlocked).
 */
int rq_remove_thread(struct thread *tp)
{
	int err;
	struct rq *rq;
	unsigned long flags;

	rq = tp->rq;
	if(current_thread() != tp) {
		raw_spin_lock_irqsave(&rq->lock, flags);
		err = raw_rq_remove_thread(rq, tp);
		raw_spin_unlock_irqrestore(&rq->lock, flags);
	} else {
		err = raw_rq_remove_thread(rq, tp);
	}

	return err;
}

static inline struct clocksource *rq_get_clock(struct rq *rq)
{
	return rq->source;
}

#ifdef CONFIG_SCHED_FAIR
/**
 * Check which of two threads has received the most time on the CPU.
 *
 * @param tp First thread, most likely the current thread.
 * @param nxt A contestant for the CPU.
 * @return True only iff the CPU time \p nxt has received is less or equal to
 *         the CPU time received by \p tp.
 */
static inline bool should_resched_cputime(struct thread *tp, struct thread *nxt)
{
	if(test_bit(THREAD_IDLE_FLAG, &nxt->flags))
		return false;
	else if(test_bit(THREAD_IDLE_FLAG, &tp->flags))
		return true;

	return nxt->cputime <= tp->cputime;
}
#else
static inline bool should_resched_cputime(struct thread *tp, struct thread *nxt)
{
	return true;
}
#endif

/**
 * @brief Update a run queue.
 * @param rq Run queue to update.
 * @note Calls sched_class::post_schedule.
 */
static void rq_update(struct rq *rq)
{
	struct sched_class *class;
	unsigned long flags = 0UL;
	struct thread *tp;

	tp = current_thread();
	flags = tp->cpu_state;
	cpu_set_state(&flags);
	class = rq->sched_class;
	if(class->post_schedule)
		class->post_schedule(rq);

	/*
	 * Last but not least, restore the interrupt state. Note that
	 * this is the last operation and it is the last operation for
	 * a reason. Therefore, rq_update must also be the last scheduling
	 * operation in the __schedule function.
	 */
	irq_restore(&tp->irq_state);
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
	unsigned long flags;
	struct rq *rq = tp->rq;

	raw_spin_lock_irqsave(&rq->lock, flags);
	set_bit(THREAD_SLEEPING_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);

	tp->timer = timer_create(rq->source, ms, &sched_sleep_timeout,
			         tp, TIMER_ONESHOT_MASK);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

/**
 * @brief Obtain the next runnable task of a run queue.
 * @param rq Run queue to get the next runnable from.
 * @return The next runnable thread. If no new thread is found, the current
 *         thread will be returned.
 */
static struct thread *sched_get_next_runnable(struct rq *rq)
{
	struct thread *next;
	struct sched_class *class = rq->sched_class;

	if(likely(class)) {
		next = class->next_runnable(rq);
		if(!next)
			next = current_thread();

		return next;
	} else {
		return current_thread();
	}
}

/**
 * @brief Destroy the kill queue.
 * @param rq Run queue containing the kill queue head.
 */
static void rq_destroy_kill_q(struct rq *rq)
{
	struct thread *walker;
	unsigned long flags;
	struct list_head *carriage, *x;

	raw_spin_lock_irq(&rq->lock, &flags);
	list_for_each_safe(carriage, x, &rq->kill_head) {
		walker = list_entry(carriage, struct thread, entry);
		raw_rq_remove_kill_thread(rq, walker);

		if(test_bit(THREAD_SYSTEM_STACK, &walker->flags))
			sched_free_stack_frame(walker);

	}
	raw_spin_unlock_irq(&rq->lock, &flags);
}

/**
 * @brief Get the number of thread switches of a given CPU.
 * @param cpu CPU to get the number of context switches for.
 * @return The number of context switches that occured on \p cpu.
 */
static unsigned long __sched_switch_count(int cpu)
{
	struct rq *rq;
	unsigned long flags;
	unsigned long num;

	rq = cpu_to_rq(cpu);
	spin_lock_irqsave(&rq->lock, flags);
	num = rq->switch_count;
	spin_unlock_irqrestore(&rq->lock, flags);

	return num;
}

/**
 * @brief Get the number of thread switches on the current CPU.
 * @return The number of context switches.
 */
unsigned long sched_switch_count(void)
{
	int cpu;

	cpu = cpu_get_id();
	return __sched_switch_count(cpu);
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
 * @see __schedule
 */
static void __hot rq_switch_context(struct rq *rq, struct thread *prev,
				    struct thread *new)
{
	struct sched_class *class = rq->sched_class;

	if(likely(prev)) {
		if(likely(test_bit(THREAD_RUNNING_FLAG, &prev->flags))) {
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
	raw_spin_unlock(&rq->lock);
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
	struct thread *current;

	qp = thread_to_queue(tp);
	tp = *tp->queue;

	raw_rq_remove_wake_thread(rq, tp);
	raw_queue_remove_thread(qp, tp);

	current = current_thread();

	if(!qp->qhead)
		qp->qhead = SIGNALED;

	if(tp->timer && tp->timer != SIGNALED) {
		timer_stop(tp->timer);
	}

	if(current != tp) {
		raw_rq_add_thread(rq, tp);
		if(prio(tp) <= prio(current))
			set_bit(THREAD_NEED_RESCHED_FLAG, &current->flags);
	} else {
		set_bit(THREAD_RUNNING_FLAG, &tp->flags);
		tp->on_rq = true;
		tp->rq = rq;
	}
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
	struct thread *iterator,
		      *tp,
		      *volatile*tpp;
	unsigned char events;
	struct list_head *carriage, *x;

	list_for_each_safe(carriage, x, &rq->wake_head) {
		iterator = list_entry(carriage, struct thread, entry);
		events = iterator->ec;

		if(events) {
			tpp = iterator->queue;
			iterator->ec--;
			tp = *tpp;
			if(tp != SIGNALED)
				rq_signal_event_queue(rq, tp);
		}

	}
}
#else
static void rq_signal_threads(struct rq *rq)
{
}
#endif

#ifdef CONFIG_PREEMPT
/**
 * @brief Reset the time slice for a thread.
 * @param tp Thread to reset the time slice for.
 *
 * The time slice of thread \p tp will be reset to \p CONFIG_TIME_SLICE.
 */
static inline void preempt_reset_slice(struct thread *tp)
{
	if(likely(tp))
		tp->slice = CONFIG_TIME_SLICE;
}
#else
static inline void preempt_reset_slice(struct thread *tp)
{
}
#endif

/**
 * @brief Search a given run queue for a thread with a name \p name.
 * @param rq Run queue to search.
 * @param name Name to look for.
 * @return Either NULL or a thread by the name of \p name.
 * @see sched_find_thread_by_name
 */
static struct thread *rq_find_thread(struct rq *rq, const char *name)
{
	unsigned long flags;
	struct thread *tp;

	raw_spin_lock_irqsave(&rq->lock, flags);
	tp = rq_get_head(rq);

	while(tp != NULL) {
		if(strcmp(tp->name, name) == 0) {
			raw_spin_unlock_irqrestore(&rq->lock, flags);
			return tp;
		}
	}

	raw_spin_unlock_irqrestore(&rq->lock, flags);
	return NULL;
}

/**
 * @brief Search all available run queue's for a thread by name.
 * @param name Name to look for.
 * @return Either NULL or a thread by the name of \p name.
 * @see rq_find_thread
 */
struct thread *sched_find_thread_by_name(const char *name)
{
	struct rq *rq;
#ifndef CONFIG_SQS
	struct thread *tp = NULL;

	RQ_FOREACH(rq) {
		tp = rq_find_thread(rq, name);
		if(tp != NULL)
			break;
	}

	return tp;
#else
	rq = sched_get_grq();
	return rq_find_thread(rq, name);
#endif
}

/**
 * @brief Update the system clock.
 * @param rq Update the clock of `rq`.
 * @see timer_process
 */
static void rq_update_clock(struct rq *rq)
{
	struct clocksource *src;

	src = rq_get_clock(rq);
	timer_process(src);
}

/**
 * @brief Prepare the a reschedule.
 * @param rq Runqueue that is about to be rescheduled.
 * @param prev Thread that lost the CPU.
 * @param irqs IRQ flags of the thread that is being scheduled out.
 *
 * This function is responsible for handling the dynamic priority and time
 * slice resets/updates.
 */
static inline void __schedule_prepare(struct rq *rq,
				      struct thread *prev,
				      struct thread *next,
				      unsigned long irqs)
{
	unsigned long flags = 0UL;

	/*
	 * Update the dynamic priorities of all threads that still
	 * reside on the run queue.
	 */
	dyn_prio_update(rq);

	/*
	 * Reset the priorities of the thread that just lost the CPU and
	 * the thread that is about to receive CPU time.
	 */
	dyn_prio_reset(prev);
	dyn_prio_reset(next);

	/*
	 * Reset the time slices of the entering and leaving threads.
	 */
	preempt_reset_slice(next);
	preempt_reset_slice(prev);
	cpu_get_state(&flags);
	prev->cpu_state = flags;
	prev->irq_state = irqs;
}

/**
 * @brief Mark a thread for termination.
 * @param tp Thread to be terminated.
 * @see sched_remote_kill_if thread_destroy
 *
 * The thread will be removed from the system the next time the
 * scheduler attempts to run it.
 */
void sched_mark_remote_kill(struct thread *tp)
{
	struct thread *cur;
	struct rq *rq;
	unsigned long flags;
	struct sched_class *class;

	rq = tp->rq;
	cur = current_thread_for(rq);

	if(cur == tp) {
		class = rq->sched_class;
		set_bit(THREAD_EXIT_FLAG, &tp->flags);
		set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
		clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
#ifdef CONFIG_EXTENDED_THREAD
		raw_event_notify_broadcast(&tp->joinq);
#endif
		raw_spin_lock_irqsave(&rq->lock, flags);
		rq_add_kill_thread(rq, tp);
		raw_spin_unlock_irqrestore(&rq->lock, flags);

		if(class->kill)
			class->kill(tp);

		return;
	}

	set_bit(THREAD_REMOTE_KILL, &tp->flags);

	if(!test_bit(THREAD_SLEEPING_FLAG, &tp->flags) &&
			!test_bit(THREAD_WAITING_FLAG, &tp->flags)) {
		cur = current_thread();
		set_bit(THREAD_NEED_RESCHED_FLAG, &cur->flags);
	}
}

/**
 * @brief Kill the current thread if its marked for removal.
 * @param rq The run queue that we're on.
 * @see __schedule sched_mark_remote_kill thread_destroy
 *
 * Appends the current thread to the kill queue and prepares it for
 * a reschedule.
 */
static void sched_remote_kill_if(struct rq *rq)
{
	struct thread *tp;
	struct sched_class *class;

	tp = current_thread_for(rq);
	if(likely(!test_bit(THREAD_REMOTE_KILL, &tp->flags)))
		return;

	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	set_bit(THREAD_EXIT_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);

	rq_add_kill_thread(rq, tp);
	class = rq->sched_class;

	if(class->kill)
		class->kill(tp);

#ifdef CONFIG_EXTENDED_THREAD
	raw_event_notify_broadcast(&tp->joinq);
#endif
}

/**
 * @brief Reschedule policy.
 * @param curr Current thread.
 * @param next Thread which is supposed to be running after \p curr.
 * @return an integer indicating whether to reschedule or not.
 * @retval 1 if __schedule should reschedule.
 * @retval 0 if __schedule should not reschedule.
 *
 * When the next runnable thread is the idle thread and only
 * \p PREEMPT_NEED_RESCHED_FLAG is set, the timeslice is reset and no
 * reschedule operation occurs.
 *
 * If the next thread is _not_ the idle thread, the return value \p rv
 * will be:\n\n
 *
 * \f$ f(rv) = x \lor y\f$\n\n
 * Where:
 * * \f$x\f$ is the \p PREEMPT_NEED_RESCHED_FLAG
 * * \f$y\f$ is the \p THREAD_NEED_RESCHED_FLAG
 *
 * When preemption is not enabled the value of THREAD_NEED_RESCHED_FLAG is
 * returned.
 */
static int __schedule_need_resched(struct thread *curr, struct thread *next)
{
#ifdef CONFIG_PREEMPT
	int preempt;

	if(likely(test_and_clear_bit(THREAD_NEED_RESCHED_FLAG,
					&curr->flags))) {
		clear_bit(PREEMPT_NEED_RESCHED_FLAG, &curr->flags);
		return true;
	}

	preempt = test_and_clear_bit(PREEMPT_NEED_RESCHED_FLAG, &curr->flags);
	if(unlikely(thread_is_idle(next) && preempt)) {
		preempt_reset_slice(curr);
		return false;
	}

	return preempt;
#else
	return test_and_clear_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags);
#endif
}

#ifdef CONFIG_PREEMPT
/**
 * @brief Check if the algorithm wants to preempt the current thread.
 * @param rq Run queue we are on.
 * @param cur Current thread.
 * @param nxt Next thread.
 *
 * When the algorithm wants to preempt the current thread, the
 * \p PREEMPT_NEED_RESCHED_FLAG will be set.
 */
static void preempt_check(struct rq *rq, struct thread *cur, struct thread *nxt)
{
	struct sched_class *class = rq->sched_class;

	if(class->preempt_chk(rq, cur, nxt) ||
			should_resched_cputime(cur, nxt)) {
		set_bit(PREEMPT_NEED_RESCHED_FLAG, &cur->flags);
	}
}
#else
#define preempt_check(__rq, __cur, __nxt)
#endif

/**
 * @brief Reschedule the current run queue.
 * @param cpu ID of the CPU which should be rescheduled.
 * @note This function also updates:
 * 	   - threads signaled from an IRQ;
 * 	   - timers;
 * 	   - threads which have used up their time slice;
 * 	   - the kill queue of the run queue;
 * @see __schedule_need_resched
 * @return Boolean value indicating whether a reschedule occurred or not.
 *
 * struct rq::lock will be locked (and unlocked).
 */
static bool __hot __schedule(int cpu)
{
	struct rq *rq;
	struct thread *next,
		      *prev;
	unsigned long flags;
	bool rescheduled = false;

	cpu_notify(SCHED_ENTER);
	rq = cpu_to_rq(cpu);
	raw_spin_lock_irq(&rq->lock, &flags);
	prev = current_thread();

	/*
	 * Run the sched clock and wake up threads that received an event
	 * from an IRQ.
	 */
	rq_update_clock(rq);
	rq_signal_threads(rq);

	next = sched_get_next_runnable(rq);
	preempt_check(rq, prev, next);

	/*
	 * Only reschedule if we have to. The decision is based on the
	 * THREAD_NEED_RESCHED_FLAG and the PREEMPT_NEED_RESCHED_FLAG. Also,
	 * if prev == next a reschedule is redundant.
	 */
	if(likely(__schedule_need_resched(prev, next) && prev != next)) {
		__sched_set_current_thread(rq, next);
		rq->switch_count++;
		rescheduled = true;

		__schedule_prepare(rq, prev, next, flags);
		rq_switch_context(rq, prev, next);
		rq_update(rq); /* restores CPU / IRQ states */

	} else {
		raw_spin_unlock_irq(&rq->lock, &flags);
	}

	rq_destroy_kill_q(rq);
	sched_remote_kill_if(rq);
	cpu_notify(SCHED_EXIT);

	return rescheduled;
}

/**
 * @brief Check if a reschedule is required.
 * @see __schedule_need_resched
 * @return true if a reschedule is required, false otherwise.
 *
 * For a more extensive check, use #__schedule_need_resched.
 */
static inline bool need_resched()
{
	struct thread *curr = current_thread();

#ifdef CONFIG_PREEMPT
	return (test_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags) ||
		test_bit(PREEMPT_NEED_RESCHED_FLAG, &curr->flags));
#else
	return test_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags);
#endif
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
/**
 * @brief Preempt a thread from IRQ context.
 * @see __schedule preempt_schedule.
 * @warning Do not use this function outside of IRQ context.
 *
 * This function is the entry point to the scheduler from locations where
 * the interrupts are already disabled. The most notable use of this function
 * is the 'off IRQ return' preemption calls.
 */
void __hot preempt_schedule_irq(void)
{
	int cpu;

	if(preempt_count())
		return;

	do {
		cpu = cpu_get_id();
		__schedule(cpu);
	} while(need_resched());
	return;
}

/**
 * @brief Reschedule the current run queue with preemption in mind.
 * @note Applications generally shouldn't call this function.
 * @see schedule __schedule
 * @warning Do not use this function in IRQ context.
 *
 * This function is the preemption entry point for in-application preemption
 * (such as preemption from preempt_enable).
 */
void __hot preempt_schedule(void)
{
	/* we don't want to preempt the current process if either
	 * a) the interrupts are disabled or
	 * b) the preemption counter != 0
	 */
	if(likely(!preemptible()))
		return;

	schedule();
	return;
}
#endif

static struct thread idle_thread, main_thread;
static void *main_stack_ptr;
static void *idle_stack_ptr;

extern void *main_stack_ptr_end;
extern void *main_stack_ptr_start;

extern void *idle_stack_ptr_end;
extern void *idle_stack_ptr_start;

THREAD(idle_thread_func, arg)
{
	struct thread *tp = arg;

	main_stack_ptr = &main_stack_ptr_start;
	irq_enable();

	thread_initialise(&main_thread, "main", &main_thread_func, &main_thread,
			CONFIG_STACK_SIZE, main_stack_ptr, SCHED_DEFAULT_PRIO);
	preempt_disable();

	while(true) {
		set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
		schedule();
#ifdef CONFIG_IDLE_SLEEP
		power_set_mode(POWER_IDLE);
		hibernate();
#endif
	}
}

/**
 * @brief Start the scheduler.
 * @note This function doesn't return.
 *
 * The arch init initialises the scheduler using this function during boot.
 */
void sched_start(void)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	idle_thread.rq = rq;
	sched_set_current_thread(&idle_thread);

	cpu_switch_context(rq, NULL, &idle_thread);
}

#ifdef CONFIG_PREEMPT
/**
 * @brief Check whether a thread is the idle thread or not.
 * @param tp Thread to check.
 * @retval false If \p tp is not the idle thread.
 * @retval true If \p tp is the idle thread.
 */
static bool thread_is_idle(struct thread *tp)
{
	if(!tp)
		return false;

	return test_bit(THREAD_IDLE_FLAG, &tp->flags);
}

/**
 * @brief Get a pointer to the preemption counter of the current thread.
 * @return A pointer to the preemption counter of the current thread.
 *
 * Returns struct thread::preempt_cnt.
 */
int *preempt_counter_ptr(void)
{
	struct thread *tp = current_thread();
	return &tp->preempt_cnt;
}

/**
 * @brief Substract \p num from the preemption counter of the current thread.
 * @param num Amount to substract from the preemption counter.
 *
 * Substracts \p num from struct thread::preempt_cnt.
 */
void __preempt_add(int num)
{
	volatile int *preempt_ptr = preempt_counter_ptr();

	*preempt_ptr += num;
}

/**
 * @brief Add \p num to the preemption counter of the current thread.
 * @param num Amount to add to the preemption counter.
 *
 * Will add \p num to struct thread::preempt_cnt.
 */
void __preempt_sub(int num)
{
	volatile int *preempt_ptr = preempt_counter_ptr();

	*preempt_ptr -= num;
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
	struct thread *next,
		      *curr;

	preempt_disable();
	next = sched_get_next_runnable(rq);
	curr = current_thread();

	if(preempt_should_resched() || prio(next) <= prio(curr))
		set_bit(THREAD_NEED_RESCHED_FLAG, &curr->flags);
	preempt_enable_no_resched();

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
	struct thread *tp;

	tp = current_thread();
	return test_bit(PREEMPT_NEED_RESCHED_FLAG, &tp->flags) ||
		should_resched();
}
#else
bool preempt_should_resched(void)
{
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
	return test_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
}

#ifdef CONFIG_SCHED_DBG
/**
 * @brief Print the threads on the run queue of \p this CPU.
 */
void print_rq(void)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	rq->sched_class->print_rq(rq);
}
#endif

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

/**
 * @brief Initialise the scheduling core.
 * @note This function is automatically called during the sysinit.
 *
 * This function initialises, but not starts, the scheduler.
 */
static void __used sched_init(void)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	rq->source = sched_get_clock();

	if(sys_sched_class.init)
		sys_sched_class.init();

	idle_stack_ptr = kzalloc(CONFIG_IDLE_STACK_SIZE);
	sched_init_idle(&idle_thread, "idle", &idle_thread_func,
			&idle_thread, CONFIG_IDLE_STACK_SIZE, idle_stack_ptr);
}

subsys_init(sched_init);

/* @} */
