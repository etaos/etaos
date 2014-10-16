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

/**
 * @file etaos/sched.h
 */

#ifndef __SCHED_H__
#define __SCHED_H__

/**
 * @addtogroup sched
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/time.h>
#include <etaos/thread.h>
#include <etaos/bitops.h>

#ifndef CONFIG_TIME_SLICE
#define CONFIG_TIME_SLICE 5
#endif

struct rq;
struct thread_queue;
/**
 * @struct sched_class etaos/sched.h
 * @brief Scheduling class descriptor.
 *
 * A scheduling class describes a scheduling algorithm. Most notable, how to
 * insert and remove threads or to get the next runnable task.
 */
struct sched_class {
	/** @brief Get the next runnable task. */
	struct thread *(*next_runnable)(struct rq*);
	/** @brief post schedule. */
	void (*post_schedule)(struct rq*);
#ifdef CONFIG_DYN_PRIO
	/**< @brief update the dynamic prio. */
	void (*dyn_prio_update)(struct rq*);
#endif
	/** @brief Add a new thread to the run queue. */
	void (*add_thread)(struct rq *rq, struct thread *tp);
	/** @brief Remove a thread from the run queue. */
	int (*rm_thread)(struct rq *rq, struct thread *tp);
#ifdef CONFIG_EVENT_MUTEX
	/** @brief Get the next thread after \p tp.
	 * @param tp Thread to get the 'next' of.
	 */
	struct thread *(*thread_after)(struct thread *tp);
#endif
#ifdef CONFIG_THREAD_QUEUE
	/** @brief Add a thread to the queue */
	void (*queue_add)(struct thread_queue *q, struct thread *tp);
	/** @brief Remove a thread from a queue. */
	void (*queue_rm)(struct thread_queue *q, struct thread *tp);
#endif
};

/**
 * @struct rr_rq
 * @brief Round robin run queue.
 */
struct rr_rq {
	/** @brief Run queue head. */
	struct thread *run_queue;
};

/**
 * @struct rq etaos/sched.h
 * @brief Run queue descriptor.
 *
 * Each CPU has a run queue. A run queue is a collection of threads which
 * are ready to receive CPU time. Each time a new thread has to be scheduled
 * the scheduler picks a new thread from the run queue of the executing CPU.
 */
struct rq {
	/** @brief System scheduling class */
	struct sched_class *sched_class;
#ifdef CONFIG_RR
	/** @brief Round robin run queue */
	struct rr_rq rq;
#endif
	/**
	 * @brief Wake queue.
	 *
	 * All threads which are put in a waiting queue are also listed on
	 * this queue. Whenever an interrupt signals the queue, the scheduler
	 * will notice by checking this queue.
	 */
	struct thread *wake_queue;
	/** @brief Kill queue. */
	struct thread *kill_queue;
	/** @brief Thread currently running on this run queue */
	struct thread *current;

	/** @brief Amount of thread switches done */
	unsigned long switch_count;

	/** @brief CPU clock source */	
	struct clocksource *source;
	/** @brief Run queue lock */
	spinlock_t lock;
};

/**
 * @def sched_need_resched
 * @brief Check if the scheduler needs to reschedule a thread.
 * @param __t Thread which needs to be checked.
 */
#define sched_need_resched(__t) test_bit(THREAD_NEED_RESCHED_FLAG, \
				&__t->flags)

extern unsigned char prio(struct thread *tp);
extern void schedule(void);
extern bool should_resched(void);
/**
 * @ingroup archAPI
 * @brief Get the run queue of the current CPU.
 * @pure
 */
extern struct rq *sched_get_cpu_rq(void);
/**
 * @ingroup archAPI
 * @brief Select a run queue to add a thread to.
 * @return The selected run queue.
 * @pure
 *
 * The most effecient run queue is returned (i.e. the run queue with the least
 * amount of threads already on it.
 */
extern struct rq *sched_select_rq(void);
/**
 * @ingroup archAPI
 * @brief Reschedule the CPU.
 * @param rq Run queue requesting the context switch.
 * @param prev Previously running thread.
 * @param next Thread to switch to.
 * @pure
 *
 * Do a context switch to the new given thread.
 */
extern void cpu_reschedule(struct rq *rq, 
		struct thread *prev, 
		struct thread *next);
/**
 * @ingroup archAPI
 * @brief Setup the stack from of a thread.
 * @param tp Thread to setup a stack for.
 * @param stack Pointer to the base of the stack.
 * @param stack_size Length of \p stack.
 * @param handle Function pointer to the thread handle.
 * @pure
 */
extern void sched_create_stack_frame(struct thread *tp, stack_t *stack,
				size_t stack_size, thread_handle_t handle);
/**
 * @ingroup archAPI
 * @brief Free a stack frame.
 * @param tp Thread to free the stackframe of.
 * @warning The thread should NEVER be executed again after a call to this
 *          function.
 */
extern void sched_free_stack_frame(struct thread *tp);

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
extern void rq_add_thread_no_lock(struct thread *tp);


extern void sched_setup_sleep_thread(struct thread *tp, unsigned ms);

extern void sched_init(void);

/**
 * @brief System scheduling policy
 */
extern struct sched_class sys_sched_class;
#ifdef CONFIG_THREAD_QUEUE
extern void queue_remove_thread(struct thread_queue *qp, struct thread *tp);
extern void queue_add_thread(struct thread_queue *qp, struct thread *tp);

/**
 * @brief Get the thread_queue the given thread is on.
 * @param tp Thread to get the thread_queue for.
 * @note You have to be sure that the thread is on one. If its not, a false
 *       thread queue is returned.
 */
static inline struct thread_queue *thread_to_queue(struct thread *tp)
{
	struct thread **tpp = (struct thread**)tp->queue;

	if(tpp)
		return container_of(tpp, struct thread_queue, qhead);
	else
		return NULL;
}
#endif

/**
 * @brief Remove a thread from its wake queue.
 * @param tp Thrad to remove
 */
static inline void thread_remove_from_wake_q(struct thread *tp)
{
	if(tp->rq)
		rq_remove_wake_thread(tp->rq, tp);
}

/**
 * @brief Remove a thread from its kill queue.
 * @param tp Thread to remove.
 */
static inline void thread_remove_from_kill_q(struct thread *tp)
{
	if(tp->rq)
		rq_remove_kill_thread(tp->rq, tp);
}

/* @} */
#endif

