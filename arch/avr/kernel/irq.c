/*
 *  ETA/OS - AVR IRQ support
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

#define AVR_IRQ_CORE 1

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/bitops.h>
#include <etaos/list.h>

#include <asm/io.h>
#include <asm/irq.h>

void arch_irq_disable(void)
{
	cli();
}

void arch_irq_enable(void)
{
	sei();
}

unsigned long arch_irq_get_flags(void)
{
	return ((unsigned long) (SREG & AVR_IRQ_BITS));
}

void arch_irq_restore_flags(unsigned long *flags)
{
	if(test_bit(AVR_IRQ_FLAG, flags))
		sei();
	return;
}

SIGNAL(TIMER0_OVERFLOW_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();

	chip->chip_handle(TIMER0_OVERFLOW_VECTOR_NUM);
}

