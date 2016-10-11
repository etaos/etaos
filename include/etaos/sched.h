/*
 *  ETA/OS - Sched header
 *  Copyright (C) 2014, 2015, 2016  Michel Megens <dev@bietje.net>
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
	/** 
	 * @brief Get the next runnable task.
	 * @param rq Run queue to get the next runnable task from
	 */
	struct thread *(*next_runnable)(struct rq*);
	/**
	 * @brief Initialise the scheduling class.
	 */
	void (*init)(void);
	/**
	 * @brief Kill a given thread.
	 * @param tp Thread which is going to be killed.
	 * @note Do not free up the memory yet. Leave this to the scheduler.
	 */
	void (*kill)(struct thread *tp);
	/** 
	 * @brief Post schedule.
	 * @param rq RQ which has just received a schedule call.
	 *
	 * This function pointer, if present is called after the context
	 * switch has been done.
	 */
	void (*post_schedule)(struct rq*);
#ifdef CONFIG_DYN_PRIO
	/** 
	 * @brief update the dynamic priority of all threads on the given rq.
	 * @param rq Run queue to update.
	 * @param num Numeral value that is added to struct thread::dprio.
	 */
	void (*dyn_prio_update)(struct rq* rq, int num);
#endif
	/** 
	 * @brief Add a new thread to the run queue.
	 * @param rq RQ to add to.
	 * @param tp Thread to add.
	 */
	void (*add_thread)(struct rq *rq, struct thread *tp);
	/** 
	 * @brief Remove a thread from the run queue.
	 * @param rq Run queue to remove from.
	 * @param tp Thread to remove.
	 * @return Error code.
	 * @retval -EINVAL on error.
	 * @retval -EOK on success.
	 */
	int (*rm_thread)(struct rq *rq, struct thread *tp);
#ifdef CONFIG_EVENT_MUTEX
	/** 
	 * @brief Get the next thread after \p tp.
	 * @param tp Thread to get the 'next' of.
	 */
	struct thread *(*thread_after)(struct thread *tp);
#endif
#ifdef CONFIG_PREEMPT
	/**
	 * @brief Check if a thread needs to be preempted.
	 * @param rq Run queue we're on.
	 * @param cur Current thread which could be preempted
	 * @return True if we need to preempt, false otherwise.
	 */
	bool (*preempt_chk)(struct rq *rq, struct thread *cur);
#endif
#ifdef CONFIG_THREAD_QUEUE
	/** 
	 * @brief Add a thread to the queue
	 * @param q Queue to add to.
	 * @param tp Thread to add.
	 */
	void (*queue_add)(struct thread_queue *q, struct thread *tp);
	/** 
	 * @brief Remove a thread from a queue.
	 * @param q Queue to remove from.
	 * @param tp Thread to remove.
	 */
	void (*queue_rm)(struct thread_queue *q, struct thread *tp);
#endif
};

#ifdef CONFIG_RR_ENTITY
#define __DEFINE_RQ .rr_rq = { .run_queue = NULL }
#endif

#define DEFINE_RQ(__name,__class)			\
	struct rq __name = {				\
		.sched_class = __class,			\
		.lock = SPIN_LOCK_INIT(__name.lock),	\
		__DEFINE_RQ,				\
	}

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
#if defined(CONFIG_RR) || defined(CONFIG_FIFO) || defined(CONFIG_LOTTERY)
	/** @brief Round robin run queue */
	struct rr_rq rr_rq;
#endif
	/** @brief Number of threads in the run queue */
	unsigned long num;
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

CDECL

extern unsigned char prio(struct thread *tp);
extern void schedule(void);
extern bool should_resched(void);

/**
 * @ingroup archAPI
 * @brief Get the ID of the current CPU.
 */
extern int cpu_get_id(void);
/**
 * @ingroup archAPI
 * @brief Get the scheduling clock source.
 * @return The scheduling clock source.
 */
extern struct clocksource *sched_get_clock(void);
/**
 * @ingroup archAPI
 * @brief Get the RQ of the given cpu.
 * @param cpu CPU to get the run queue from.
 * @return The run queue of \p cpu.
 */
extern struct rq *cpu_to_rq(int cpu);

#ifdef CONFIG_SQS
/**
 * @ingroup archAPI
 * @brief Get the global run queue
 * @return The global run queue.
 */
extern struct rq *sched_get_grq(void);
#endif

/**
 * @ingroup archAPI
 * @brief Get the run queue of the current CPU.
 */
extern struct rq *sched_get_cpu_rq(void);
/**
 * @ingroup archAPI
 * @brief Select a run queue to add a thread to.
 * @return The selected run queue.
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
 *
 * Do a context switch to the new given thread.
 */
extern void cpu_switch_context(struct rq *rq, 
				struct thread *prev, 
				struct thread *next);
/**
 * @ingroup archAPI
 * @brief Setup the stack from of a thread.
 * @param tp Thread to setup a stack for.
 * @param stack Pointer to the base of the stack.
 * @param stack_size Length of \p stack.
 * @param handle Function pointer to the thread handle.
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
extern void sched_yield(struct rq *rq);
extern void sched_start(void);

#if defined(CONFIG_SYS_EDF)
extern struct sched_class edf_class;
#define sys_sched_class edf_class
#elif defined(CONFIG_SYS_RR)
extern struct sched_class rr_class;
#define sys_sched_class rr_class

#elif defined(CONFIG_SYS_LOTTERY)
extern struct sched_class lottery_class;
#define sys_sched_class lottery_class

#elif defined(CONFIG_SYS_FIFO)

extern struct sched_class fifo_class;
#define sys_sched_class fifo_class
#endif

#ifdef CONFIG_THREAD_QUEUE
extern void queue_remove_thread(struct thread_queue *qp, struct thread *tp);
extern void queue_add_thread(struct thread_queue *qp, struct thread *tp);
extern void thread_queue_wait(struct thread_queue *qp, unsigned int ms);
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
CDECL_END

/* @} */
#endif

