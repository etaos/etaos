/*
 *  ETA/OS - FIFO sched class
 *  Copyright (C) 2014, 2015  Michel Megens <dev@michelmegens.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied wafifoanty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/thread.h>

#include "rr_shared.h"

/**
 * @addtogroup fifo
 * @{
 */

/**
 * @brief Insert a new thread into a queue.
 * @param tpp Root queue pointer.
 * @param tp Thread to insert.
 */
static void fifo_queue_insert(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;
#ifdef CONFIG_EVENT_MUTEX
	tp->ec = 0;
#endif
	tp->queue = tpp;
	
	thread = *tpp;

	if(thread == SIGNALED) {
		thread = NULL;
#ifdef CONFIG_EVENT_MUTEX
		tp->ec++;
#endif
	} else if(thread) {
		while(thread) {
			tpp = &thread->se.next;
			thread = thread->se.next;
		}
	}

	tp->se.next = thread;
	*tpp = tp;
#ifdef CONFIG_EVENT_MUTEX
	if(tp->se.next && tp->se.next->ec) {
		tp->ec += tp->se.next->ec;
		tp->se.next->ec = 0;
	}
#endif
}

#ifdef CONFIG_THREAD_QUEUE
/**
 * @brief Add a new thread to a queue.
 * @param qp Queue to add the thread to.
 * @param tp Thread to add.
 */
static void fifo_thread_queue_add(struct thread_queue *qp, struct thread *tp)
{
	fifo_queue_insert(&qp->qhead, tp);
}

/**
 * @brief Remove a thread from a queue.
 * @param qp Queue to remove from.
 * @param tp Thread to remove.
 */
static void fifo_thread_queue_remove(struct thread_queue *qp, struct thread *tp)
{
	rr_shared_queue_remove(&qp->qhead, tp);
}
#endif

/**
 * @brief Add a new thread to the run queue.
 * @param rq Run queue to add to.
 * @param tp Thread to add.
 */
static void fifo_add_thread(struct rq *rq, struct thread *tp)
{
	rq->num++;
	fifo_queue_insert(&rq->rr_rq.run_queue, tp);
}

/**
 * @brief Remove a thread from a rq.
 * @param rq Run queue to remove from.
 * @param tp Thread to remove.
 * @retval -EOK on success.
 * @return -EINVAL on error (no thread was removed).
 */
static int fifo_rm_thread(struct rq *rq, struct thread *tp)
{
	rq->num--;
	return rr_shared_queue_remove(&rq->rr_rq.run_queue, tp);
}

/**
 * @brief Get the next runnable thread on the run queue.
 * @param rq RQ to get the next runnable thread from.
 * @return The next runnable thread.
 * @retval NULL if no runnable thread was found.
 */
static struct thread *fifo_next_runnable(struct rq *rq)
{
	struct thread *runnable;

	for(runnable = rq->rr_rq.run_queue; runnable; 
			runnable = runnable->se.next) {
		if(!test_bit(THREAD_RUNNING_FLAG, &runnable->flags))
			continue;
		else
			break;
	}
	
	return runnable;
}

#ifdef CONFIG_EVENT_MUTEX
/**
 * @brief Get the next runnable thread after \p tp.
 * @param tp Thread to get the 'next' of.
 * @return The next of \p tp.
 * @retval NULL if \p tp has no next thread.
 */
static struct thread *fifo_thread_after(struct thread *tp)
{
	if(tp)
		return tp->se.next;
	else
		return NULL;
}
#endif

/**
 * @brief FIFO scheduling class.
 */
struct sched_class fifo_class = {
	.init = NULL,
	.rm_thread = &fifo_rm_thread,
	.add_thread = &fifo_add_thread,
	.next_runnable = &fifo_next_runnable,
#ifdef CONFIG_EVENT_MUTEX
	.thread_after = &fifo_thread_after,
#endif
	.post_schedule = NULL,
#ifdef CONFIG_THREAD_QUEUE
	.queue_add = &fifo_thread_queue_add,
	.queue_rm = &fifo_thread_queue_remove,
#endif
};

/* @} */
