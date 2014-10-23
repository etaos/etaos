/*
 *  ETA/OS - AVR CPU
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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
#include <etaos/irq.h>
#include <etaos/list.h>
#include <etaos/tick.h>

#include <asm/io.h>
#include <asm/cpu.h>

static struct irq_chip avr_irq_chip = {
	.name = "avr_irq_chip",
	.irqs = STATIC_INIT_LIST_HEAD(avr_irq_chip.irqs),
	.chip_handle = &irq_handle,
	.sleep = NULL,
	.resume = NULL,
};

/**
 * @brief Get the AVR IRQ chip.
 * @return ATmega AVR IRQ chip.
 */
struct irq_chip *arch_get_irq_chip(void)
{
	return &avr_irq_chip;
}

/**
 * @brief Start the AVR system clock.
 * @param irq IRQ vector number.
 * @param src Clocksource structure for the AVR sysclk.
 */
void avr_start_sysclk(int irq, struct clocksource *src)
{
	systick_setup(irq, src);
#if F_CPU == 16000000
	OCR0A = 250;
#elif F_CPU == 8000000
	OCRA0 = 125;
#else
#error Unsupported CPU frequency for timer IRQ
#endif
	TIMSK0 = TOIE0;
	TCCR0A = WGM00 | WGM01;
	TCCR0B = WGM02 | CS00 | CS01;
}
/* @} */

