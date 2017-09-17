/*
 *  ETA/OS - AVR IRQ support
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

#define AVR_IRQ_CORE 1

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/irq.h>
#include <etaos/bitops.h>
#include <etaos/list.h>
#include <etaos/stdio.h>
#include <etaos/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/pgm.h>

void arch_irq_disable(void)
{
	cli();
}

void arch_irq_enable(void)
{
	sei();
}

void arch_local_irq_enable(void)
{
	arch_irq_enable();
}

void arch_local_irq_disable(void)
{
	arch_irq_disable();
}

unsigned long arch_irq_get_flags(void)
{
	unsigned char flags;

	__asm__ __volatile__(
			"in %0, %1"	"\n\t"
			"andi %0, %2"
			: "=&r" (flags)
			: "I" (AVR_STATUS_ADDR), "M" (AVR_IRQ_BITS)
			: "memory"
			);

	return (unsigned long)flags;
}

void arch_irq_restore_flags(unsigned long *flags)
{
	if(test_bit(AVR_IRQ_FLAG, flags))
		sei();
	return;
}

void raw_irq_enabled_flags(unsigned long *flags)
{
	unsigned char status;

	__asm__ __volatile__(
			"in %0, %1"	"\n\t"
			: "=&r" (status)
			: "M" (AVR_STATUS_ADDR)
			: "memory"
			);
	*flags = (status & (1UL << AVR_INTERRUPT_FLAG)) != 0;
}

#define EXT_IRQ0_SHIFT 0
#define EXT_IRQ1_SHIFT 2
#define EXT_IRQ2_SHIFT 4
#define EXT_IRQ3_SHIFT 6
#define EXT_IRQ4_SHIFT 0
#define EXT_IRQ5_SHIFT 2
#define EXT_IRQ6_SHIFT 4
#define EXT_IRQ7_SHIFT 6

void cpu_request_irq(struct irq_data *data)
{
	uint8_t flags;

	if(test_bit(IRQ_FALLING_FLAG, &data->flags)) {
		flags = 0x2;
	} else if(test_bit(IRQ_RISING_FLAG, &data->flags)) {
		flags = 0x3;
	} else {
		flags = 0x1;
		data->flags |= IRQ_RISING_MASK | IRQ_FALLING_MASK;
	}

	switch(data->irq) {
#ifdef EXT_IRQ0_VECTOR_NUM
	case EXT_IRQ0_VECTOR_NUM:
		EICRA |= flags << EXT_IRQ0_SHIFT;
		EIMSK |= 1;
		break;
#endif

#ifdef EXT_IRQ1_VECTOR_NUM
	case EXT_IRQ1_VECTOR_NUM:
		EICRA |= flags << EXT_IRQ1_SHIFT;
		EIMSK |= 2;
		break;
#endif

#ifdef EXT_IRQ2_VECTOR_NUM
	case EXT_IRQ2_VECTOR_NUM:
		EICRA |= flags << EXT_IRQ2_SHIFT;
		EIMSK |= 4;
		break;
#endif

#ifdef EXT_IRQ3_VECTOR_NUM
	case EXT_IRQ3_VECTOR_NUM:
		EICRA |= flags << EXT_IRQ3_SHIFT;
		EIMSK |= 8;
		break;
#endif

#ifdef EXT_IRQ4_VECTOR_NUM
	case EXT_IRQ4_VECTOR_NUM:
		EICRB |= flags << EXT_IRQ4_SHIFT;
		EIMSK |= 16;
		break;
#endif

#ifdef EXT_IRQ5_VECTOR_NUM
	case EXT_IRQ5_VECTOR_NUM:
		EICRB |= flags << EXT_IRQ5_SHIFT;
		EIMSK |= 32;
		break;
#endif

#ifdef EXT_IRQ6_VECTOR_NUM
	case EXT_IRQ6_VECTOR_NUM:
		EICRB |= flags << EXT_IRQ6_SHIFT;
		EIMSK |= 64;
		break;
#endif

#ifdef EXT_IRQ7_VECTOR_NUM
	case EXT_IRQ7_VECTOR_NUM:
		EICRB |= flags << EXT_IRQ7_SHIFT;
		EIMSK |= 128;
		break;
#endif

	default:
		return;
	}
}

int cpu_trigger_irq(struct irq_data *data)
{
	struct gpio_pin *pin = data->pin;
	int falling, rising;

	if(!pin)
		return -EINVAL;

	falling = test_bit(IRQ_FALLING_FLAG, &data->flags);
	rising = test_bit(IRQ_RISING_FLAG, &data->flags);

	if(falling && rising) {
		gpio_pin_write(pin, !data->value);
		data->value = !data->value;
	} else if(falling) {
		if(!data->value)
			gpio_pin_write(pin, true);

		gpio_pin_write(pin, false);
		data->value = false;
	} else if(rising) {
		if(data->value)
			gpio_pin_write(pin, false);

		gpio_pin_write(pin, true);
		data->value = true;
	}
	
	return -EOK;
}
