/*
 *  ETA/OS - Thread header
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

#ifndef __THREAD_H__
#define __THREAD_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/list.h>

#ifdef CONFIG_THREAD_QUEUE
#define DEFINE_THREAD_QUEUE(__name)			 \
	static struct thread_queue __name = {		 \
		.sched_class = &sys_sched_class,	 \
		.lock = STATIC_SPIN_LOCK_INIT,		 \
		.qhead = SIGNALED,			 \
	}

#define INIT_THREAD_QUEUE {			 \
		.sched_class = &sys_sched_class, \
		.lock = STATIC_SPIN_LOCK_INIT,	 \
		.qhead = SIGNALED,		 \
	}
#endif

typedef void (*thread_handle_t)(void *arg);

#define THREAD(fn, param) \
static void fn(void * param); \
static void fn(void *param)

struct rr_entity {
	struct thread *next;
};

#ifdef CONFIG_THREAD_QUEUE
struct thread_queue {
	struct sched_class *sched_class;
	spinlock_t lock;
	struct thread *qhead;
};
#endif

struct rq;
struct thread {
	const char *name;
	unsigned long flags;
#ifdef CONFIG_PREEMPT
	unsigned int slice;
#endif
	int preemt_cnt;

	bool on_rq;
	struct rq *rq;

	struct thread *volatile*queue;
	struct thread *rq_next; /* wake/kill list */

	void    *stack;
	stack_t *sp;
	size_t stack_size;
	unsigned char prio;
#ifdef CONFIG_TIMER	
	struct timer *timer;
#endif
#ifdef CONFIG_EVENT_MUTEX
	unsigned char ec;
#endif

	void *param;
#ifdef CONFIG_RR
	struct rr_entity se;
#endif
};


#define THREAD_RUNNING_FLAG	 0
#define THREAD_SLEEPING_FLAG	 1
#define THREAD_WAITING_FLAG	 2
#define THREAD_EXIT_FLAG 	 3
#define THREAD_NEED_RESCHED_FLAG 4

struct sched_class;
extern void thread_wake_up_from_irq(struct thread *t);
extern int thread_initialise(struct thread *tp, char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack, unsigned char prio);

extern struct thread *thread_create(char *name, thread_handle_t handle, 
		void *arg, size_t stack_size, void *stack, unsigned char prio);
extern struct sched_class sys_sched_class;

extern void sched_init_idle(struct thread *tp, char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack);

extern void yield(void);
extern void sleep(unsigned ms);

#endif /* __THREAD_H__ */
