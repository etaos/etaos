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
#include <etaos/time.h>
#include <etaos/spinlock.h>

typedef void (*thread_handle_t)(void *arg);

#define THREAD(__name, __argname) static void __name(void * __argname)

struct sched_class;
struct thread_queue {
	struct sched_class *sclass;
	spinlock_t lock;
	volatile struct thread *queue;
};

struct thread {
	const char *name;
	struct thread *next;
	unsigned long flags;
	int preemt_cnt;

	struct thread *q_next;
	struct thread *volatile*queue;

	void *stack;
	void *sp;
	size_t stack_size;
	unsigned char prio;
	
	struct timer *timer;
	unsigned char ec;

	void *param;
};

#define THREAD_RUNNING_FLAG	 0
#define THREAD_SLEEPING_FLAG	 1
#define THREAD_WAITING_FLAG	 2
#define THREAD_EXIT_FLAG 	 3
#define THREAD_NEED_RESCHED_FLAG 4

extern void thread_wake_up_from_irq(struct thread *t);
extern int thread_initialise(struct thread *tp, char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack, unsigned char prio);

extern struct thread *thread_create(char *name, thread_handle_t handle, 
		void *arg, size_t stack_size, void *stack, unsigned char prio);

#endif /* __THREAD_H__ */
