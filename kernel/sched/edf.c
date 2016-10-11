/*
 *  ETA/OS - Earliest deadline first algorithm
 *  Copyright (C) 2016  Michel Megens <dev@bietje.net>
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
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/thread.h>

#include "rr_shared.h"

static void edf_init(void)
{
}

static inline int edf_calc_ratio(unsigned char prio)
{
	return ((13 * prio) / 24) + 10;
}

static bool edf_sort_before(time_t node, time_t this)
{
	return this <= node;
}

static time_t deadline(struct rr_entity *entity)
{
	return entity->deadline;
}

static int raw_edf_insert(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;
	struct rr_entity *se;
	struct clocksource *clock;

#ifdef CONFIG_EVENT_MUTEX
	tp->ec = 0;
#endif

	clock = sched_get_clock();
	se = &tp->se;
	tp->queue = tpp;
	thread = *tpp;
	se->deadline = clocksource_get_tick(clock) + edf_calc_ratio(prio(tp));

	if(thread == SIGNALED) {
		thread = NULL;
#ifdef CONFIG_EVENT_MUTEX
		tp->ec++;
#endif
	} else if(thread) {
		while(thread &&
			!edf_sort_before(deadline(&thread->se), deadline(se))) {
			tpp = &thread->se.next;
			thread = thread->se.next;
		}
	}
	
	se->next = thread;
	*tpp = tp;

#ifdef CONFIG_EVENT_MUTEX
	if(se->next && se->next->ec) {
		tp->ec += se->next->ec;
		se->next->ec = 0;
	}
#endif

	return -EOK;
}

#ifdef CONFIG_THREAD_QUEUE
/**
 * @brief Add a new thread to a queue.
 * @param qp Queue to add the thread to.
 * @param tp Thread to add.
 */
static void edf_thread_queue_add(struct thread_queue *qp, struct thread *tp)
{
	raw_edf_insert(&qp->qhead, tp);
}

/**
 * @brief Remove a thread from a queue.
 * @param qp Queue to remove from.
 * @param tp Thread to remove.
 */
static void edf_thread_queue_remove(struct thread_queue *qp, struct thread *tp)
{
	rr_shared_queue_remove(&qp->qhead, tp);
}
#endif

/**
 * @brief Add a new thread to the run queue.
 * @param rq Run queue to add to.
 * @param tp Thread to add.
 */
static void edf_add_thread(struct rq *rq, struct thread *tp)
{
	struct rr_rq *rr;

	rr = &rq->rr_rq;
	if(raw_edf_insert(&rr->run_queue, tp) == -EOK)
		rq->num++;
}

/**
 * @brief Remove a thread from a rq.
 * @param rq Run queue to remove from.
 * @param tp Thread to remove.
 * @retval -EOK on success.
 * @return -EINVAL on error (no thread was removed).
 */
static int edf_rm_thread(struct rq *rq, struct thread *tp)
{
	int rc;

	if((rc = rr_shared_queue_remove(&rq->rr_rq.run_queue, tp)) == -EOK)
		rq->num--;
	
	return rc;
}

/**
 * @brief Get the next runnable thread on the run queue.
 * @param rq RQ to get the next runnable thread from.
 * @return The next runnable thread.
 * @retval NULL if no runnable thread was found.
 */
static struct thread *edf_next_runnable(struct rq *rq)
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

#ifdef CONFIG_PREEMPT
static bool edf_preempt_chk(struct rq *rq, struct thread *cur)
{
	struct thread *nxt;

	nxt = edf_next_runnable(rq);
	return edf_sort_before(deadline(&cur->se), deadline(&nxt->se));
}
#endif

#ifdef CONFIG_EVENT_MUTEX
/**
 * @brief Get the next runnable thread after \p tp.
 * @param tp Thread to get the 'next' of.
 * @return The next of \p tp.
 * @retval NULL if \p tp has no next thread.
 */
static struct thread *edf_thread_after(struct thread *tp)
{
	if(tp)
		return tp->se.next;
	else
		return NULL;
}
#endif

struct sched_class edf_class = {
	.init = &edf_init,
	.rm_thread = &edf_rm_thread,
	.add_thread = &edf_add_thread,
	.next_runnable = &edf_next_runnable,
#ifdef CONFIG_PREEMPT
	.preempt_chk = &edf_preempt_chk,
#endif
#ifdef CONFIG_EVENT_MUTEX
	.thread_after = &edf_thread_after,
#endif
	.post_schedule = NULL,
#ifdef CONFIG_THREAD_QUEUE
	.queue_add = &edf_thread_queue_add,
	.queue_rm = &edf_thread_queue_remove,
#endif
};
