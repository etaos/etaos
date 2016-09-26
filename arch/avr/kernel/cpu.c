/*
 *  ETA/OS - AVR CPU
 *  Copyright (C) 2014, 2015   Michel Megens <dev@bietje.net>
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
 * @addtogroup atmega
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/bitops.h>
#include <etaos/irq.h>
#include <etaos/list.h>
#include <etaos/tick.h>
#include <etaos/hrtimer.h>

#include <asm/io.h>

static struct irq_chip avr_irq_chip = {
	.name = "avr_irq_chip",
	.irqs = STATIC_INIT_LIST_HEAD(avr_irq_chip.irqs),
	.chip_handle = &irq_handle,
	.sleep = NULL,
	.resume = NULL,
};

int cpu_get_id(void)
{
	return 0;
}

/**
 * @brief Get the AVR IRQ chip.
 * @return ATmega AVR IRQ chip.
 */
struct irq_chip *arch_get_irq_chip(void)
{
	return &avr_irq_chip;
}

/* @} */

