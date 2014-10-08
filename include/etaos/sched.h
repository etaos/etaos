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
	struct thread *threads;
	struct thread *kill_queue;

	bool (*sort)(struct thread *head, struct thread *entry);
	struct thread *(*next_runnable)(struct rq*);
};

struct rq {
	struct sched_class *sclass;
	struct thread *rq_threads;
	struct thread *current;
	struct clocksource *source;
	spinlock_t lock;
	unsigned int switch_count;
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

extern void class_remove_thread(struct rq *rq, struct thread *tp);
extern void class_add_thread(struct rq *rq, struct thread *tp);

extern void sched_remove_from_queue(struct thread *thread, 
		struct thread_queue *qp);
extern void sched_add_to_queue(struct thread *thread, struct thread_queue *qp);

extern void raw_rq_add_thread(struct rq *rq, struct thread *tp);
extern void raw_rq_remove_thread(struct rq *rq, struct thread *tp);

extern void rq_add_thread(struct rq *rq, struct thread *tp);
extern void rq_remove_thread(struct rq *rq, struct thread *tp);

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
	*preemt_counter_ptr(tp) += 1;
}

static inline void preemt_disable(struct thread *tp)
{
	++*preemt_counter_ptr(tp);
}
#endif
