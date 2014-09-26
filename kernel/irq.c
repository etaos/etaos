/*
 *  ETA/OS - IRQ support
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
#include <etaos/irq.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/mem.h>

#include <asm/io.h>

static void *arch_irqs[CONFIG_ARCH_VECTORS];

static inline struct irq_data *irq_to_data(int irq)
{
	if(irq >= CONFIG_ARCH_VECTORS)
		return NULL;
	
	return arch_irqs[irq];
}

static inline int irq_store_data(int irq, struct irq_data *data)
{
	if(irq >= CONFIG_ARCH_VECTORS)
		return -EINVAL;
	if(arch_irqs[irq]) /* already initialised */
		return -EINVAL;

	arch_irqs[irq] = data;
	return -EOK;
}

void irq_save_and_disable(unsigned long *flags)
{
	*flags = arch_irq_get_flags();
	arch_irq_disable();
}

void irq_restore(unsigned long *flags)
{
	if(!(*flags))
		return;
	arch_irq_restore_flags(flags);
}

int irq_request(int irq, unsigned long flags)
{
	struct irq_data *idata;

	idata = irq_to_data(irq);
	if(!idata) {
		idata = kmalloc(sizeof(*idata));
		irq_store_data(irq, idata);
	}

	idata->irq = irq;
	idata->flags = flags & (IRQ_RISING_MASK | IRQ_FALLING_MASK);
	set_bit(IRQ_ENABLE_FLAG, &idata->flags);
	return -EOK;
}

int irq_set_handle(int irq, irq_vector_t vector)
{
	struct irq_data *idata;

	idata = irq_to_data(irq);
	if(!idata)
		return -EINVAL;

	idata->handler = vector;
	return -EOK;
}

