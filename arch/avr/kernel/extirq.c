/*
 *  ETA/OS - External IRQ handlers
 *  Copyright (C) 2017 <dev@bietje.net>
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
#include <etaos/preempt.h>

#include <asm/irq.h>
#include <asm/io.h>

#ifdef EXT_IRQ0_VECTOR
SIGNAL(EXT_IRQ0_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ0_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef EXT_IRQ1_VECTOR
SIGNAL(EXT_IRQ1_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ1_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef EXT_IRQ2_VECTOR
SIGNAL(EXT_IRQ2_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ2_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef EXT_IRQ3_VECTOR
SIGNAL(EXT_IRQ3_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ3_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef EXT_IRQ4_VECTOR
SIGNAL(EXT_IRQ4_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ4_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef EXT_IRQ5_VECTOR
SIGNAL(EXT_IRQ5_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ5_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef EXT_IRQ6_VECTOR
SIGNAL(EXT_IRQ6_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ6_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif

#ifdef EXT_IRQ7_VECTOR
SIGNAL(EXT_IRQ7_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(EXT_IRQ7_VECTOR_NUM);
	preempt_schedule_irq();
}
#endif
