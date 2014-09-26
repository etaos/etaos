/*
 *  ETA/OS - IRQ header
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

#ifndef __IRQ_H__
#define __IRQ_H__

extern void irq_save_and_disable(unsigned long *flags);
extern void irq_restore(unsigned long *flags);

/* arch functions */
void arch_irq_restore_flags(unsigned long *flags);
unsigned long arch_irq_get_flags();
void arch_irq_disable();
void arch_irq_enable();

#define irq_disable() arch_irq_disable()
#define irq_enable() arch_irq_enable()

struct irq_data;
typedef enum {
	IRQ_NONE = 0 << 0,
	IRQ_HANDLED = 1 << 0,
	IRQ_WAKE_OWNER = 1 << 1,
} irqreturn_t;
typedef irqreturn_t (*irq_vector_t)(struct irq_data *irq, void *data);

struct irq_data {
	unsigned int irq;
	unsigned int vector;

	unsigned long flags;
	struct irq_chip *chip;

	irq_vector_t handler;
	void *private_data;
};
struct irq_chip {
};



#endif

