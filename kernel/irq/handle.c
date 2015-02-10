/*
 *  ETA/OS - IRQ handling
 *  Copyright (C) 2014, 2015   Michel Megens
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

/** @} */

