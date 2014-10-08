/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014   Michel Megens
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/irq.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/spinlock.h>

#include <asm/io.h>

void thread_wake_up_from_irq(struct thread *thread)
{
}

static inline struct thread *sched_next_runnable(struct rq *rq)
{
	if(rq->sclass)
		return rq->sclass->next_runnable(rq);

	return NULL;
}

static void rq_post_schedule(struct rq *rq)
{
}

static inline struct thread *sched_get_next_runnable(struct rq *rq)
{
	struct sched_class *class = rq->sclass;

	if(class)
		return class->next_runnable(rq);
	else
		return NULL;
}

#define current(_rq) ((_rq)->current)
static void rq_schedule(void)
{
	struct rq *rq;
	int64_t diff;
	struct thread *tp;
	unsigned long flags;
#ifdef CONFIG_EVENT_MUTEX
	struct thread *carriage, *volatile*tpp;
	unsigned char events;
#endif

	rq = sched_get_cpu_rq();

resched:
	preemt_disable(current(rq));
	spin_lock_irqsave(&rq->lock, flags);
#ifdef CONFIG_EVENT_MUTEX
	carriage = rq->sclass->threads;
	while(carriage) {
		events = carriage->ec;

		if(events) {
			tpp = carriage->queue;
			carriage->ec--;
			tp = *tpp;
			if(tp != SIGNALED)
				thread_wake_up_from_irq(tp);
		}
		carriage = carriage->next;
	}
#endif

#ifdef CONFIG_TIMER
	diff = tm_update_source(rq->source);
	if(diff)
		tm_process_clock(rq->source, diff);
#endif

	tp = sched_get_next_runnable(rq);
	if(test_and_clear_bit(THREAD_NEED_RESCHED_FLAG,
				&current(rq)->flags) && tp != current(rq)) {
		rq->current = tp;
		rq->switch_count++;
		cpu_reschedule(rq, current(rq), tp);
		spin_unlock_irqrestore(&rq->lock, flags);
		rq_post_schedule(rq);
		rq = sched_get_cpu_rq();
	} else {
		spin_unlock_irqrestore(&rq->lock, flags);
	}

	preemt_enable_no_resched(current(rq));
	if(test_bit(THREAD_NEED_RESCHED_FLAG, &current(rq)->flags))
		goto resched;

	return;
}

void class_add_thread(struct rq *rq, struct thread *tp)
{
	struct sched_class *class;

	if(!rq || !tp)
		return;

	class = rq->sclass;

	tp->next = class->threads;
	class->threads = tp;
}

void class_remove_thread(struct rq *rq, struct thread *tp)
{
	struct thread *carriage, *prev;
	struct sched_class *class = rq->sclass;

	for(carriage = class->threads, prev = NULL; carriage; 
			prev = carriage, carriage = carriage->next) {
		if(carriage == tp) {
			prev->next = carriage->next;
			tp->next = NULL;
			break;
		}	
	}

	tp->queue = SIGNALED;
	return;
}

static void __sched_remove_from_queue(struct thread *thread, 
		struct thread *volatile*qpp)
{
	struct thread *tp;

	irq_enter_critical();
	tp = *qpp;
	irq_exit_critical();

	if(tp != SIGNALED) {
		while(tp) {
			if(tp == thread) {
				irq_enter_critical();
				*qpp = thread->q_next;
#ifdef CONFIG_EVENT_MUTEX
				if(thread->ec) {
					if(thread->q_next)
						thread->q_next->ec = thread->ec;
					thread->ec = 0;
				}
#endif
				irq_exit_critical();
				thread->q_next = NULL;
				thread->queue = NULL;
				break;
			}

			qpp = &tp->q_next;
			tp = tp->q_next;
		}
	}

	return;
}

#define QUEUE_POINTER(_q) ((struct thread *volatile*)&(_q)->queue)
static void __sched_add_to_queue(struct thread *thread,
		struct thread *volatile*qpp, struct sched_class *class)
{
	struct thread *tp;

	thread->ec = 0;
	thread->queue = qpp;

	irq_enter_critical();
	tp = *qpp;

	if(tp == SIGNALED) {
		tp = NULL;
		thread->ec++;
	} else if(tp) {
		irq_exit_critical();
		while(tp && !class->sort(tp, thread)) {
			qpp = &tp->q_next;
			tp = tp->next;
		}
		irq_enter_critical();
	}

	thread->next = tp;
	*qpp = thread;

	if(thread->next) {
		if(thread->next->ec) {
			thread->ec += thread->next->ec;
			thread->next->ec = 0;
		}
	}

	irq_exit_critical();
	return;
}

#define RQ_POINTER(_rq) ((struct thread *volatile*)&rq->rq_threads)
void raw_rq_add_thread(struct rq *rq, struct thread *tp)
{
	struct sched_class *class;

	class = rq->sclass;
	__sched_add_to_queue(tp, RQ_POINTER(rq), class);
}

void raw_rq_remove_thread(struct rq *rq, struct thread *tp)
{
	struct thread *volatile*tqpp;

	tqpp = RQ_POINTER(rq);
	__sched_remove_from_queue(tp, tqpp);
}

void rq_add_thread(struct rq *rq, struct thread *tp)
{
	spin_lock(&rq->lock);
	raw_rq_add_thread(rq, tp);
	spin_unlock(&rq->lock);
}

void rq_remove_thread(struct rq *rq, struct thread *tp)
{
	spin_lock(&rq->lock);
	raw_rq_remove_thread(rq, tp);
	spin_unlock(&rq->lock);
}

void sched_remove_from_queue(struct thread *thread, struct thread_queue *qp)
{
	spin_lock(&qp->lock);
	__sched_remove_from_queue(thread, QUEUE_POINTER(qp));
	spin_unlock(&qp->lock);
}

void sched_add_to_queue(struct thread *thread, struct thread_queue *qp)
{
	spin_lock(&qp->lock);
	__sched_add_to_queue(thread, QUEUE_POINTER(qp), qp->sclass);
	spin_unlock(&qp->lock);
}

void schedule(void)
{
	rq_schedule();
}

