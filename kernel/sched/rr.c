/*
 *  ETA/OS - Round robin sched class
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
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/thread.h>

static void rr_queue_insert(struct thread *volatile*tpp, struct thread *tp)
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
		while(thread && prio(thread) <= prio(tp)) {
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

static int rr_queue_remove(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;
	int err = -EINVAL;

	thread = *tpp;

	if(thread == SIGNALED)
		return err;

	while(thread) {
		if(thread == tp) {
			err = 0;
			*tpp = tp->se.next;
#ifdef CONFIG_EVENT_MUTEX
			if(tp->ec) {
				if(tp->se.next)
					tp->se.next->ec = tp->ec;
				tp->ec = 0;
			}
#endif

			tp->se.next = NULL;
			tp->queue = NULL;
			break;
		}

		tpp = &thread->se.next;
		thread = thread->se.next;
	}

	return err;
}

#ifdef CONFIG_THREAD_QUEUE
static void rr_thread_queue_add(struct thread_queue *qp, struct thread *tp)
{
	rr_queue_insert(&qp->qhead, tp);
}

static void rr_thread_queue_remove(struct thread_queue *qp, struct thread *tp)
{
	rr_queue_remove(&qp->qhead, tp);
}
#endif

static void rr_add_thread(struct rq *rq, struct thread *tp)
{
	rr_queue_insert(&rq->rq.run_queue, tp);
}

static int rr_rm_thread(struct rq *rq, struct thread *tp)
{
	return rr_queue_remove(&rq->rq.run_queue, tp);
}

static struct thread *rr_next_runnable(struct rq *rq)
{
	struct thread *runnable;

	for(runnable = rq->rq.run_queue; runnable; 
			runnable = runnable->se.next) {
		if(!test_bit(THREAD_RUNNING_FLAG, &runnable->flags))
			continue;
		else
			break;
	}
	
	return runnable;
}

#ifdef CONFIG_EVENT_MUTEX
static struct thread *rr_thread_after(struct thread *tp)
{
	if(tp)
		return tp->se.next;
	else
		return NULL;
}
#endif

#ifdef CONFIG_DYN_PRIO
static void rr_update_dyn_prio(struct rq *rq)
{
	struct thread *walker;

	walker = rq->rq.run_queue;

	while(walker) {
		walker->dprio += 1;
		walker = walker->se.next;
	}
}
#endif

struct sched_class sys_sched_class = {
	.rm_thread = &rr_rm_thread,
	.add_thread = &rr_add_thread,
	.next_runnable = &rr_next_runnable,
#ifdef CONFIG_DYN_PRIO
	.dyn_prio_update = &rr_update_dyn_prio,
#endif
#ifdef CONFIG_EVENT_MUTEX
	.thread_after = &rr_thread_after,
#endif
	.post_schedule = NULL,
#ifdef CONFIG_THREAD_QUEUE
	.queue_add = &rr_thread_queue_add,
	.queue_rm = &rr_thread_queue_remove,
#endif
};

