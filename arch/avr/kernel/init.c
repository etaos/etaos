/*
 *  Eta/OS - AVR5 arch boot
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

#include <etaos/kernel.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>
#include <etaos/mem.h>
#include <etaos/init.h>

#include <asm/sched.h>
#include <asm/time.h>
#include <asm/io.h>
#include <asm/cpu.h>
#include <asm/simulavr.h>
#include <asm/usart.h>

extern void avr_init(void);
extern void avr_install_irqs(void);
extern char __heap_start;
static const char *mm_heap_start = &__heap_start;

void avr_init(void)
{
#ifdef CONFIG_MALLOC
	size_t hsize = RAMEND - CONFIG_STACK_SIZE - ((size_t)mm_heap_start);
	mm_init((void*)mm_heap_start, hsize);
#endif

#ifdef CONFIG_VFS
	vfs_init();
#endif
#if defined(CONFIG_ATMEGA_USART) || defined(CONFIG_ATMEGA_USART_MODULE)
	atmega_usart_init();
#endif
#ifdef CONFIG_TIMER
	avr_timer_init();
#endif
	irq_enable();
#ifdef CONFIG_SCHED
	avr_init_sched();
	sched_init();
#else
	main_init();
#endif

	while(1);
}
