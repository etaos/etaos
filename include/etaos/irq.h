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

#include <etaos/kernel.h>
#include <etaos/types.h>

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
	struct list_head irq_list;

	unsigned long flags;
	struct irq_chip *chip;

	irq_vector_t handler;
	void *private_data;
};

#define IRQ_ENABLE_FLAG 0
#define IRQ_RISING_FLAG 1
#define IRQ_FALLING_FLAG 2

#define IRQ_RISING_MASK (1 << IRQ_RISING_FLAG)
#define IRQ_FALLING_MASK (1 << IRQ_FALLING_FLAG)

struct irq_chip {
	const char *name;
	struct list_head irqs;

	void (*sleep)(struct irq_chip *chip);
	void (*resume)(struct irq_chip *chip);
};

extern int irq_chip_add_irq(struct irq_chip *chip, struct irq_data *irq);
extern int irq_chip_init(struct irq_chip *chip, const char *name);
extern int irq_request(int irq, unsigned long flags);
extern int irq_set_handle(int irq, irq_vector_t vector);

/* IRQ CHIP FUNCTIONS */

extern struct irq_chip *arch_get_irq_chip(void);

static inline void irq_chip_set_sleep(struct irq_chip *chip,
				      void (*sleep)(struct irq_chip *))
{
	chip->sleep = sleep;
}

static inline void irq_chip_set_resume(struct irq_chip *chip,
				      void (*resume)(struct irq_chip *))
{
	chip->resume = resume;
}
#endif

