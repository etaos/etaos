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

/**
 * @file etaos/thread.h
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/list.h>

/**
 * @addtogroup thread
 */
/** @{ */

struct sched_class;

CDECL
/**
 * @brief System scheduling class.
 *
 * The sys_sched_class is a pointer to the scheduling class configured
 * at compile time.
 */
extern struct sched_class sys_sched_class;
#ifdef CONFIG_THREAD_QUEUE
/**
 * @brief Define a new thread queue.
 * @param __name Name of the thread queue.
 */
#define DEFINE_THREAD_QUEUE(__name)			 \
	struct thread_queue __name = {		 	 \
		.sched_class = &sys_sched_class,	 \
		.lock = STATIC_SPIN_LOCK_INIT,		 \
		.qhead = SIGNALED,			 \
	}

/**
 * @brief Initialise a new thread queue.
 *
 * The sched_class, lock and qhead members will be initialised by this
 * macro.
 */
#define INIT_THREAD_QUEUE {			 \
		.sched_class = &sys_sched_class, \
		.lock = STATIC_SPIN_LOCK_INIT,	 \
		.qhead = SIGNALED,		 \
	}

#endif

CDECL_END

/**
 * @brief Thread handle type definition.
 * @param arg Thread argument.
 */
typedef void (*thread_handle_t)(void *arg);

/**
 * @brief Define a thread.
 * @param fn Function name of the thread.
 * @param param Name of the thread function parameter.
 *
 * The function will be defined ass:
   @code{.c}
   static void fn(void *param)
   @endcode
 */
#define THREAD(fn, param) \
static void fn(void * param); \
static void fn(void *param)

/**
 * @struct rr_entity
 * @brief Round robin entity.
 */
struct rr_entity {
	struct thread *next; //!< List entry pointer.
};

#ifdef CONFIG_THREAD_QUEUE
/**
 * @struct thread_queue
 * @brief Thread queue descriptor
 */
struct thread_queue {
	/**
	 * @brief Sched class organizing the queue.
	 */
	struct sched_class *sched_class;
	/**
	 * @brief queue lock.
	 */
	spinlock_t lock;
	/**
	 * @brief Head of the queue.
	 */
	struct thread *qhead;
};

#endif

struct rq;
/**
 * @struct thread
 * @brief Thread descriptor.
 * @ingroup thread
 *
 * All information available for a thread is stored in this structure.
 */
struct thread {
	const char *name; //!< Name of the thread.
	unsigned long flags; //!< Thread flags
#ifdef CONFIG_PREEMPT
	unsigned int slice; //!< Time slice.
	int preemt_cnt; //!< Preempt enable counter.
#endif

	bool on_rq; //!< Run queue enable.
	struct rq *rq; //!< Run queue pointer.

	struct thread *volatile*queue; //!< Queue root pointer.
	struct thread *rq_next; //!< Wake/Kill list entry.

	void    *stack; //!< Root stack pointer.
	stack_t *sp; //!< Run time stack pointer.
	size_t stack_size; //!< Size of the stack.
	unsigned char prio; //!< Thread priority.
#ifdef CONFIG_DYN_PRIO
	unsigned char dprio; //!< Dynamic thread priority.
#endif
#ifdef CONFIG_TIMER	
	struct timer *timer; //!< Event timer.
#endif
#ifdef CONFIG_EVENT_MUTEX
	unsigned char ec; //!< Event counter.
#endif

	void *param; //!< Thread parameter.
#if defined(CONFIG_RR) || defined(CONFIG_FIFO)
	struct rr_entity se; //!< Scheduling entity.
#endif
};

/**
 * @name Thread flags
 * @{
 */
#define THREAD_RUNNING_FLAG	 0 //!< Thread running flag.
#define THREAD_SLEEPING_FLAG	 1 //!< Thread sleeping flag.
#define THREAD_WAITING_FLAG	 2 //!< Thread waiting flag.
#define THREAD_EXIT_FLAG 	 3 //!< Thread waiting to be killed.
#define THREAD_NEED_RESCHED_FLAG 4 //!< Thread nees a resched.
/** @} */

CDECL

extern void thread_wake_up_from_irq(struct thread *t);
extern int thread_initialise(struct thread *tp, const char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack, unsigned char prio);

extern struct thread *thread_create(const char *name, thread_handle_t handle, 
		void *arg, size_t stack_size, void *stack, unsigned char prio);

extern void sched_init_idle(struct thread *tp, const char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack);
extern struct thread *current_thread();

extern void yield(void);
extern void sleep(unsigned ms);
extern void kill(void);
extern void wait(void);
extern void signal(struct thread *tp);
extern unsigned char nice(unsigned char prio);

#ifdef CONFIG_THREAD_QUEUE
extern void thread_queue_init(struct thread_queue *qp);
#endif

CDECL_END

#endif /* __THREAD_H__ */

/** @} */
