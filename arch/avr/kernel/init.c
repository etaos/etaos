/*
 *  Eta/OS - AVR5 arch boot
 *  Copyright (C) 2014, 2015   Michel Megens <dev@bietje.net>
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
 * @addtogroup atmega
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>
#include <etaos/mem.h>
#include <etaos/init.h>

#include <asm/sched.h>
#include <asm/timer.h>
#include <asm/io.h>
#include <asm/simulavr.h>
#include <asm/usart.h>
#include <asm/init.h>

extern void avr_init(void);
extern void avr_install_irqs(void);
extern char __heap_start;

void *main_stack_ptr = NULL;
static const char *mm_heap_start = &__heap_start;

extern void __attribute__((noinline)) dev_init(void);

/**
 * @brief AVR system initialisation.
 *
 * avr_init initialises the ATmega AVR system. It is called from the AVR
 * bootstrap.
 */
void avr_init(void)
{
#ifdef CONFIG_SCHED
	unsigned char stack_addr_low, stack_addr_hi;
#endif

#ifdef CONFIG_MALLOC
#ifdef CONFIG_EXT_MEM
	avr_sre();
#endif

#ifdef CONFIG_SCHED
	mm_init((void*)mm_heap_start, INTERNAL_RAMEND + CONFIG_EXT_MEM_SIZE -
			((size_t)mm_heap_start));

	main_stack_ptr = kmalloc(CONFIG_STACK_SIZE);
	stack_addr_low = ((size_t)main_stack_ptr + (CONFIG_STACK_SIZE - 1)) & 0xFF;
	stack_addr_hi = ((size_t)main_stack_ptr + (CONFIG_STACK_SIZE - 1)) >> 8;

	__asm__ __volatile__(
			"out %0, %2" 	"\n\t"
			"out %1, %3"	"\n\t"
			:
			: "I" (AVR_STACK_LOW_ADDR), "I" (AVR_STACK_HI_ADDR),
			"r" (stack_addr_low), "r" (stack_addr_hi)
			: "memory"
			);
#else
	mm_init((void*)mm_heap_start, INTERNAL_RAMEND + CONFIG_EXT_MEM_SIZE -
			((size_t)mm_heap_start) - CONFIG_STACK_SIZE);
#endif /* CONFIG_SCHED */
#endif /* CONFIG_MALLOC */

	dev_init();
	kinit();
	while(1);
}

void finalize_init(void)
{
}

/* @} */

