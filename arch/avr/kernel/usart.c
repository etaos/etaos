/*
 *  ETA/OS - AVR USART support
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>

#include <asm/io.h>
#include <asm/irq.h>

SIGNAL(USART_RX_STC_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(USART_RX_STC_NUM);
}

#ifdef USART1_UDRE_VECTOR
SIGNAL(USART1_UDRE_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	
	chip->chip_handle(USART1_UDRE_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef USART1_RX_COMPLETE_VECTOR
SIGNAL(USART1_RX_COMPLETE_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();

	chip->chip_handle(USART1_RX_COMPLETE_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif
