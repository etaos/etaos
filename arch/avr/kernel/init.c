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
#include <etaos/vfs.h>

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
static void *mm_heap_start = &__heap_start;

#ifndef CONFIG_EXT_MEM
#define CONFIG_EXT_MEM_SIZE 0
#endif

extern void __attribute__((noinline)) dev_init(void);

/**
 * @brief AVR system initialisation.
 *
 * avr_init initialises the ATmega AVR system. It is called from the AVR
 * bootstrap.
 */
void avr_init(void)
{
#ifdef CONFIG_MALLOC
#ifdef CONFIG_EXT_MEM
	avr_sre();
#endif

#ifdef CONFIG_SCHED
	raw_mm_heap_add_block(mm_heap_start, INTERNAL_RAMEND -
			CONFIG_INIT_STACK_SIZE -
			((size_t)mm_heap_start));
#ifdef CONFIG_EXT_MEM
	raw_mm_heap_add_block((void*)(INTERNAL_RAMEND + 1), CONFIG_EXT_MEM_SIZE);
#endif

	main_stack_ptr = kzalloc(CONFIG_STACK_SIZE);
#else
	mm_init((void*)mm_heap_start, INTERNAL_RAMEND + CONFIG_EXT_MEM_SIZE -
			((size_t)mm_heap_start) - CONFIG_STACK_SIZE);
#endif /* CONFIG_SCHED */
#endif /* CONFIG_MALLOC */

#ifdef CONFIG_VFS
	vfs_init();
#endif
	dev_init();
	kinit();
	while(1);
}

void finalize_init(void)
{
	mm_heap_add_block((void*)(INTERNAL_RAMEND - CONFIG_INIT_STACK_SIZE),
			CONFIG_INIT_STACK_SIZE);
}

/* @} */

