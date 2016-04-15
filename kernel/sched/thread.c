/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014, 2015  Michel Megens <dev@michelmegens.net>
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
 * @addtogroup thread
 */
/*@{*/

#include <etaos/kernel.h>
#include <etaos/stdlib.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/irq.h>
#include <etaos/spinlock.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/string.h>

/**
 * @brief Thread initialise backend.
 * @param tp Thread pointer.
 * @param name Name of the thread.
 * @param handle Thread handle function pointer.
 * @param arg Thread argument.
 * @param stack_size Size of the stack.
 * @param stack Pointer to the stack.
 * @param prio Priority of the thread.
 */
static void raw_thread_init(struct thread *tp, const char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack, unsigned char prio)
{
	memset(tp, 0, sizeof(*tp));
	memcpy(tp->name, name, strlen(name) + 1);

	tp->param = arg;
	tp->prio = prio;
	tp->flags = 0;
	tp->rq = NULL;
	tp->on_rq = false;
#ifdef CONFIG_EVENT_MUTEX
	tp->ec = 0;
#endif

	tp->rq_next = NULL;
	tp->queue = NULL;
	tp->timer = NULL;
#ifdef CONFIG_PREEMPT
	tp->slice = CONFIG_TIME_SLICE;
	tp->preemt_cnt = 0;
#endif
#ifdef CONFIG_DYN_PRIO
	tp->dprio = 0;
#endif
#ifdef CONFIG_SYS_LOTTERY
	list_head_init(&tp->se.tickets);
#endif

	sched_create_stack_frame(tp, stack, stack_size, handle);

	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	barrier();
}

void sched_init_idle(struct thread *tp, const char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack)
{
	raw_thread_init(tp, name, handle, arg, stack_size, stack, 255);
	set_bit(THREAD_IDLE_FLAG, &tp->flags);
}

/**
 * @brief Thread initialise backend.
 * @param name Name of the thread.
 * @param handle Thread handle function pointer.
 * @param arg Thread argument.
 * @param stack_size Size of the stack.
 * @param stack Pointer to the stack.
 * @param prio Priority of the thread.
 * @return A pointer to the newly created thread.
 */
struct thread *thread_create(const char *name, thread_handle_t handle, void *arg,
			size_t stack_size, void *stack, unsigned char prio)
{
	struct thread *tp;

	tp = kzalloc(sizeof(*tp));
	if(!tp)
		return NULL;

	thread_initialise(tp, name, handle, arg, stack_size, stack, prio);
	return tp;
}

/**
 * @brief Initialise a new thread.
 * @param tp Pointer to the thread which has to be initialised.
 * @param name Name of the thread.
 * @param handle Thread handle function pointer.
 * @param arg Thread argument.
 * @param stack_size Size of the stack.
 * @param stack Pointer to the stack.
 * @param prio Priority of the thread.
 */
int thread_initialise(struct thread *tp, const char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack, unsigned char prio)
{
	struct rq *rq;

	if(!tp)
		return -EINVAL;

	raw_thread_init(tp, name, handle, arg, stack_size, stack, prio);
	rq = sched_select_rq();

	preempt_disable();
	rq_add_thread(rq, tp);
	preempt_enable();
	return -EOK;
}

/**
 * @brief Kill the current thread.
 * @note This function doesn't return.
 */
void kill(void)
{
	struct thread *tp;
	struct sched_class *class;

	tp = current_thread();
	class = tp->rq->sched_class;

	set_bit(THREAD_EXIT_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
	thread_add_to_kill_q(tp);
	
	if(class->kill)
		class->kill(tp);

	schedule();
}

/**
 * @brief Put the current thread in a waiting state.
 * @note This function doesn't return untill it is signaled.
 * @see signal
 */
void wait(void)
{
	struct thread *tp;

	tp = current_thread();
	set_bit(THREAD_WAITING_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
	schedule();
}

/**
 * @brief Signal a waiting thread.
 * @param tp Thread which has to be signaled.
 * @see wait
 */
void signal(struct thread *tp)
{
	struct rq *rq;

	if(!tp)
		return;

	if(test_and_clear_bit(THREAD_WAITING_FLAG, &tp->flags))
	{
		set_bit(THREAD_RUNNING_FLAG, &tp->flags);
		rq = sched_get_cpu_rq();
		rq_add_thread(rq, tp);
		yield();
	}
}

/**
 * @brief Yield the current thread, if necessary.
 * @see schedule
 *
 * The current thread will be scheduled out if there is another thread on the
 * run queue (i.e. another thread ready to run) with a higher priority than
 * the calling thread.
 */
void yield(void)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	sched_yield(rq);
}

/**
 * @brief Set the priority of the current thread.
 * @param prio Priority to set.
 * @retval The old priority.
 */
unsigned char nice(unsigned char prio)
{
	struct thread *tp;
	unsigned char old;

	if(prio == 255)
		prio = 254;

	tp = current_thread();
	old = tp->prio;
	tp->prio = prio;
	yield();

	return old;
}

/**
 * @brief Put the current thread to sleep.
 * @param ms Miliseconds to sleep.
 */
void sleep(unsigned ms)
{
	struct thread *tp = current_thread();

	sched_setup_sleep_thread(tp, ms);
	schedule();
}

/**
 * @brief Get a pointer to the current thread.
 * @return A pointer to the current thread.
 */
struct thread *current_thread(void)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	return rq->current;
}

/* @} */
