/*
 *  ETA/OS - Time core
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
 * @file kernel/time/systick.c
 * @addtogroup tm
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/atomic.h>
#include <etaos/time.h>
#include <etaos/preempt.h>
#include <etaos/irq.h>
#include <etaos/tick.h>
#include <etaos/stdio.h>
#include <etaos/math.h>

struct clocksource *sys_clk;

/**
 * @brief System tick IRQ handler.
 * @param irq IRQ data.
 * @param data Private data.
 */
static irqreturn_t systick_irq_handle(struct irq_data *irq, void *data)
{
	struct clocksource *cs = (struct clocksource*)data;

	timer_source_inc(cs);
	preempt_schedule();
	return IRQ_HANDLED;
}

/**
 * @brief Get the system tick in seconds.
 * @return The system tick in seconds.
 */
time_t systick_get_seconds(void)
{
	time_t now = (time_t)sys_tick;
	return (time_t)(now / 1000);
}

/**
 * @brief Setup the system tick.
 * @param irq IRQ bound to the system tick.
 * @param src Clock source handling the system tick.
 */
void systick_setup(int irq, struct clocksource *src)
{
	irq_request(irq, &systick_irq_handle, IRQ_RISING_MASK, src);
}

/** @} */

