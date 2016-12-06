/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014, 2015, 2016  Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
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
#include <etaos/event.h>
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
	tp->flags = 0UL;
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
	tp->preempt_cnt = 0;
#endif
#ifdef CONFIG_DYN_PRIO
	tp->dprio = 0;
#endif
#ifdef CONFIG_SYS_LOTTERY
	list_head_init(&tp->se.tickets);
#endif
#ifdef CONFIG_EXTENDED_THREAD
	thread_queue_init(&tp->joinq);
#endif

	irq_store_flags(&tp->irq_state);
	sched_create_stack_frame(tp, stack, stack_size, handle);

	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	barrier();
}

/**
 * @brief Create the idle thread.
 * @param tp Idle thread thread pointer.
 * @param name Thread name.
 * @param handle Idle thread handle.
 * @param arg Idle thread argument.
 * @param stack_size Idle thread stack size.
 * @param stack Idle thread stack pointer.
 */
void sched_init_idle(struct thread *tp, const char *name, 
		thread_handle_t handle, void *arg, size_t stack_size, 
		void *stack)
{
	raw_thread_init(tp, name, handle, arg, stack_size, stack, 255);
	set_bit(THREAD_IDLE_FLAG, &tp->flags);
	set_bit(THREAD_SYSTEM_STACK, &tp->flags);
}

/**
 * @brief Allocate and start a new thread.
 * @param name Thread name.
 * @param handle Thread handle.
 * @param arg Thread argument.
 * @param attr Thread attributes.
 * @return The allocated thread.
 */
struct thread *thread_create(const char *name, thread_handle_t handle,
		void *arg, thread_attr_t *attr)
{
	struct thread *tp;

	tp = kzalloc(sizeof(*tp));
	if(!tp)
		return NULL;

	thread_init(tp, name, handle, arg, attr);
	return tp;
}

/**
 * @brief Initialise a new thread.
 * @param tp The thread pointer to initialise.
 * @param name Thread name.
 * @param handle Thread handle.
 * @param arg Thread argument.
 * @param attr Thread attributes.
 * @return An error code.
 */
int thread_init(struct thread *tp, const char *name, thread_handle_t handle,
		void *arg, thread_attr_t *attr)
{
	size_t stack_size;
	void *stack;
	unsigned char prio;
	bool alloc = false;
	int rv;

	prio = 0;
	stack_size = 0;
	stack = NULL;

	if(attr) {
		prio = attr->prio;
		stack = attr->stack;
		stack_size = attr->stack_size;
	}

	if(!prio)
		prio = SCHED_DEFAULT_PRIO;

	if(!stack_size)
		stack_size = CONFIG_STACK_SIZE;

	if(!stack) {
		stack = kzalloc(stack_size);
		alloc = true;
	}

	rv = thread_initialise(tp, name, handle, arg, stack_size, stack, prio);

	if(alloc)
		set_bit(THREAD_SYSTEM_STACK, &tp->flags);

	return rv;
}

/**
 * @brief Initialise a new thread.
 * @param name Thread name.
 * @param handle Thread handle.
 * @param arg Thread argument.
 * @param attr Thread attributes.
 * @return The allocated thread.
 * @note The thread \p tp is not started.
 * @see thread_start
 */
struct thread *thread_alloc(const char *name, thread_handle_t handle,
		void *arg, thread_attr_t *attr)
{
	struct thread *tp;
	size_t stack_size;
	void *stack;
	unsigned char prio;
	bool alloc;

	prio = 0;
	stack_size = 0;
	stack = NULL;
	tp = kzalloc(sizeof(*tp));

	if(!tp)
		return NULL;

	if(!prio)
		prio = SCHED_DEFAULT_PRIO;

	if(!stack_size)
		stack_size = CONFIG_STACK_SIZE;

	if(!stack) {
		stack = kzalloc(stack_size);
		alloc = true;
	}

	raw_thread_init(tp, name, handle, arg, stack_size, stack, prio);

	if(alloc)
		set_bit(THREAD_SYSTEM_STACK, &tp->flags);

	return tp;
}

/**
 * @brief Start a new thread.
 * @param tp Thread to start.
 * @see thread_init
 */
void thread_start(struct thread *tp)
{
	int cpu;
	struct rq *rq;

	if(!tp)
		return;

	cpu = cpu_get_id();
	rq = cpu_to_rq(cpu);

	preempt_disable();
	rq_add_thread(rq, tp);
	preempt_enable();
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
 * @return An error code.
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
#ifdef CONFIG_EXTENDED_THREAD
	raw_event_notify_broadcast(&tp->joinq);
#endif
	thread_add_to_kill_q(tp);
	
	if(class->kill)
		class->kill(tp);

	schedule();
}

#ifdef CONFIG_EXTENDED_THREAD
int join(struct thread *tp)
{
	if(test_bit(THREAD_EXIT_FLAG, &tp->flags))
		return -EOK;

	return event_wait(&tp->joinq, EVENT_WAIT_INFINITE);
}
#endif

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
	schedule();
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
 *
 * Returns a pointer to the thread that is running on the CPU that
 * executes this function.
 */
struct thread *current_thread(void)
{
	struct rq *rq;

	rq = sched_get_cpu_rq();
	return rq->current;
}

/* @} */
