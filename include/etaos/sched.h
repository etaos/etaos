/*
 *  ETA/OS - Sched header
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

#ifndef __SCHED_H__
#define __SCHED_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/time.h>
#include <etaos/thread.h>
#include <etaos/bitops.h>

struct rq;
struct sched_class {
	struct thread *(*next_runnable)(struct rq*);
	void (*add_thread)(struct rq *rq, struct thread *tp);
	int (*rm_thread)(struct rq *rq, struct thread *tp);
	void (*thread_init)(struct thread *tp);
#ifdef CONFIG_THREAD_QUEUE
	void (*queue_add)(struct thread_queue *q, struct thread *tp);
	void (*queue_rm)(struct thread_queue *q, struct thread *tp);
#endif
};

struct rr_rq {
	struct thread *run_queue;
};

struct rq {
	struct sched_class *sched_class;
#ifdef CONFIG_RR
	struct rr_rq rq;
#endif
	struct thread *wake_queue;
	struct thread *kill_queue;
	struct thread *current;

	unsigned int switch_count;
	
	struct clocksource *source;
	spinlock_t lock;
};


#define SIGNALED ((void*)-1)

#define sched_need_resched(__t) test_bit(THREAD_NEED_RESCHED_FLAG, \
				&(__t)->flags)


extern void schedule(void);
extern struct rq *sched_get_cpu_rq(void);
extern struct rq *sched_select_rq(void);
extern void cpu_reschedule(struct rq *rq, 
		struct thread *prev, 
		struct thread *next);

extern void raw_thread_add_to_wake_q(struct thread *tp);
extern void raw_thread_add_to_kill_q(struct thread *tp);
extern void thread_add_to_wake_q(struct thread *tp);
extern void thread_add_to_kill_q(struct thread *tp);

extern void raw_rq_remove_wake_thread(struct rq *rq, struct thread *tp);
extern void raw_rq_remove_kill_thread(struct rq *rq, struct thread *tp);
extern void rq_remove_wake_thread(struct rq *rq, struct thread *tp);
extern void rq_remove_kill_thread(struct rq *rq, struct thread *tp);
extern int raw_rq_remove_thread_noresched(struct rq *rq, struct thread *tp);

extern int rq_remove_thread(struct thread *tp);
extern int rq_add_thread(struct rq *rq, struct thread *tp);

#ifdef CONFIG_THREAD_QUEUE
extern void queue_remove_thread(struct thread_queue *qp, struct thread *tp);
extern void queue_add_thread(struct thread_queue *qp, struct thread *tp);

static inline struct thread_queue *thread_to_queue(struct thread *tp)
{
	struct thread **tpp = (struct thread**)tp->queue;

	if(tpp)
		return container_of(tpp, struct thread_queue, qhead);
	else
		return NULL;
}
#endif

extern struct sched_class sys_sched_class;

static inline struct thread *current_thread(void)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	return rq->current;
}

static inline int *preemt_counter_ptr(struct thread *tp)
{
	return &tp->preemt_cnt;
}
static inline int preemt_dec_and_test(struct thread *tp)
{
	return !--*preemt_counter_ptr(tp) && sched_need_resched(tp);
}

static inline void preemt_enable(struct thread *tp)
{
	if(preemt_dec_and_test(tp))
		schedule();
}

static inline void preemt_enable_no_resched(struct thread *tp)
{
	*preemt_counter_ptr(tp) -= 1;
}

static inline void preemt_disable(struct thread *tp)
{
	++*preemt_counter_ptr(tp);
}

static inline void thread_remove_from_wake_q(struct thread *tp)
{
	if(tp->rq)
		rq_remove_wake_thread(tp->rq, tp);
}

static inline void thread_remove_from_kill_q(struct thread *tp)
{
	if(tp->rq)
		rq_remove_kill_thread(tp->rq, tp);
}
#endif

