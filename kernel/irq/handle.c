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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/irq.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/thread.h>

static void irq_handle_hard_irq(struct irq_data *data)
{
	irqreturn_t retv;
#ifdef CONFIG_SCHED
	struct irq_thread_data *tdata;
#endif

	retv = data->handler ?
		data->handler(data, data->private_data) : IRQ_NONE;
	switch(retv) {
	case IRQ_NONE:
		break;

	case IRQ_HANDLED:
		data->num += 1;
		break;

#ifdef CONFIG_SCHED
	case IRQ_WAKE_OWNER:
		data->num += 1;
		tdata = container_of(data, struct irq_thread_data, idata);
		thread_wake_up_from_irq(tdata->thread);
		break;
#endif

	default:
		break;
	}
	
}

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
