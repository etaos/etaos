/*
 *  ETA/OS - IRQ handling
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
 * @file irq/handle.c
 * @addtogroup irq
 * @{
 */
#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/irq.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/sched.h>
#include <etaos/thread.h>

#ifdef CONFIG_IRQ_THREAD
static DEFINE_THREAD_QUEUE(irq_thread_queue);
static void irq_thread_wait();

/**
 * @brief Threaded IRQ handle.
 * @param data IRQ data.
 * @note This function is a thread handle.
 */
void irq_handle_fn(void *data)
{
	struct irq_data *irq = data;

	while(true) {
		irq_thread_wait();
		if(test_bit(THREAD_EXIT_FLAG, &current_thread()->flags))
			kill();

		irq->handler(irq, irq->private_data);
	}
}
#endif

/**
 * @brief Handle a hardware IRQ.
 * @param data IRQ which has to be handled.
 */
static void irq_handle_hard_irq(struct irq_data *data)
{
	irqreturn_t retv;
#ifdef CONFIG_IRQ_THREAD
	struct irq_thread_data *tdata;
#endif

	if(!test_bit(IRQ_THREADED_FLAG, &data->flags)) {
		retv = data->handler ?
			data->handler(data, data->private_data) : IRQ_NONE;
	} else {
		retv = IRQ_WAKE_OWNER;
	}

	switch(retv) {
	case IRQ_HANDLED:
	case IRQ_NONE:
		break;

#ifdef CONFIG_IRQ_THREAD
	case IRQ_WAKE_OWNER:
		data->num += 1;
		tdata = container_of(data, struct irq_thread_data, idata);
		tdata->owner->ec += 1;
		break;
#endif

	default:
		break;
	}
	
}

/**
 * @brief Handle an IRQ
 * @param irq IRQ which has to be handled.
 */
void irq_handle(int irq)
{
	struct irq_data *data;

	data = irq_to_data(irq);
	if(!data)
		return;
	
	if(!test_bit(IRQ_ENABLE_FLAG, &data->flags))
		return;

	irq_handle_hard_irq(data);
}

#ifdef CONFIG_IRQ_THREAD
/**
 * @brief Put an IRQ thread in a waiting state.
 * @note The thread wont wake up until it is signaled and woken up by
 *       irq_signal_threads.
 * @see irq_signal_threads schedule
 */
static void irq_thread_wait(void)
{
	struct thread *tp;

	tp = current_thread();
	queue_add_thread(&irq_thread_queue, tp);
	set_bit(THREAD_WAITING_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &tp->flags);
	clear_bit(THREAD_RUNNING_FLAG, &tp->flags);
	schedule();
}

bool testy = false;

/**
 * @brief Wake up all signaled IRQ threads.
 * @param rq Run queue to add threads to when they wake up.
 * @warning Applications shouldn't call this function.
 */
void irq_signal_threads(struct rq *rq)
{
	struct thread *walker;

	walker = irq_thread_queue.qhead;
	while(walker && walker != SIGNALED) {
		if(walker->ec) {
			queue_remove_thread(&irq_thread_queue, walker);
			rq_add_thread_no_lock(walker);
			clear_bit(THREAD_WAITING_FLAG, &walker->flags);
			set_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags);
		}
		walker = irq_thread_queue.sched_class->thread_after(walker);
	}

	if(!(&irq_thread_queue.qhead))
		irq_thread_queue.qhead = SIGNALED;
}
#endif

/** @} */

