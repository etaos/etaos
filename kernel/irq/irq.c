/*
 *  ETA/OS - IRQ support
 *  Copyright (C) 2014, 2015   Michel Megens
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
 * @file irq/irq.c
 * @addtogroup irq
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/cpu.h>
#include <etaos/mem.h>
#include <etaos/thread.h>

#include <asm/io.h>

static void *arch_irqs[CONFIG_ARCH_VECTORS];

/**
 * @brief Get the IRQ data based on a vector number.
 * @param irq The vector number.
 * @return The IRQ data that belongs to \p IRQ.
 */
struct irq_data *irq_to_data(int irq)
{
	if(irq >= CONFIG_ARCH_VECTORS)
		return NULL;
	
	return arch_irqs[irq];
}

/**
 * @brief Store IRQ data.
 * @param irq IRQ to store data for.
 * @param data IRQ data which has to be stored.
 * @return Error code.
 * @retval -EOK on success.
 * @retval -EINVAL on error.
 */
static inline int irq_store_data(int irq, struct irq_data *data)
{
	if(irq >= CONFIG_ARCH_VECTORS)
		return -EINVAL;
	
	arch_irqs[irq] = data;
	return -EOK;
}

/**
 * @brief Save IRQ flags and disable them.
 * @param flags Pointer to store the IRQ flags in.
 */
void irq_save_and_disable(unsigned long *flags)
{
	*flags = arch_irq_get_flags();
	arch_irq_disable();
}

/**
 * @brief Restore the IRQ flags.
 * @param flags IRQ flags to restore.
 */
void irq_restore(unsigned long *flags)
{
	if(!(*flags))
		return;
	arch_irq_restore_flags(flags);
}

#ifdef CONFIG_IRQ_THREAD
#define IRQ_THREAD_PRIO 20
/**
 * @brief Request a threaded IRQ.
 * @param data IRQ data.
 * @return An error code.
 */
static int irq_request_threaded_irq(struct irq_thread_data *data)
{
	void *stack;
	thread_attr_t attr;

	stack = kzalloc(CONFIG_IRQ_STACK_SIZE);
	if(!stack)
		return -ENOMEM;

	attr.prio = IRQ_THREAD_PRIO;
	attr.stack = stack;
	attr.stack_size = CONFIG_IRQ_STACK_SIZE;
	data->owner = thread_create("ithread", &irq_handle_fn, &data->idata,
			&attr);

	if(!data->owner)
		return -ENOMEM;

	return -EOK;
}
#endif

/**
 * @brief Request an IRQ.
 * @param irq IRQ vector which is to be requested.
 * @param vector IRQ handler.
 * @param flags IRQ flags.
 * @param priv Private IRQ data.
 * @return An error code. -EOK if no error ocurred.
 */
int irq_request(int irq, irq_vector_t vector, unsigned long flags,
		void *priv)
{
	struct irq_data *data;
	int err;

#ifdef CONFIG_IRQ_THREAD
	struct irq_thread_data *idata;
	if(test_bit(IRQ_THREADED_FLAG, &flags)) {
		idata = kzalloc(sizeof(*data));
		if(!idata)
			return -ENOMEM;

		data = &idata->idata;
		err = irq_request_threaded_irq(idata);
		if(err) {
			kfree(idata);
			return err;
		}
	} else {
		data = kzalloc(sizeof(*data));
	}
#else
	data = kzalloc(sizeof(*data));
#endif

	if(!data)
		return -ENOMEM;

	data->irq = irq;
	data->handler = vector;
	data->flags = flags;
	data->private_data = priv;
	data->chip = arch_get_irq_chip();
	err = irq_chip_add_irq(data->chip, data);
	irq_store_data(irq, data);
	cpu_request_irq(data);

	set_bit(IRQ_ENABLE_FLAG, &data->flags);
	return err;
}

bool in_irq_context(void)
{
	unsigned long cpu_flags;

	cpu_flags = 0UL;
	cpu_get_state(&cpu_flags);
	return test_bit(CPU_IRQ_EXEC_FLAG, &cpu_flags);
}

/**
 * @brief Set the IRQ handle function.
 * @param irq IRQ vector to set the handle for.
 * @param vector IRQ handle to set.
 * @return An error code.
 */
int irq_set_handle(int irq, irq_vector_t vector)
{
	struct irq_data *idata;

	idata = irq_to_data(irq);
	if(!idata)
		return -EINVAL;

	idata->handler = vector;
	return -EOK;
}

/** @} */

