/*
 *  ETA/OS - AVR timer handlers
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

#include <asm/io.h>
#include <asm/init.h>
#include <asm/asmdef.h>
#include <asm/irq.h>

/* define standard registers */
__tmp_reg__  = 0
__zero_reg__ = 1

.extern timer0_isr
.extern rq_update_clock
.extern preempt_schedule_irq
.extern timer_trigger_dbg_irq

.section .text

.func timer_irq_update_clocksource
timer_irq_update_clocksource:
	irq_return_save
	cli
#ifdef CONFIG_IRQ_DEBUG
	XCALL timer_trigger_dbg_irq
#endif
	XCALL rq_update_clock
#ifdef CONFIG_PREEMPT_FULL
	XCALL preempt_schedule_irq
#endif
	sei
	irq_return_restore
	ret
.endfunc

.global TIMER0_OVERFLOW_VECTOR
.type TIMER0_OVERFLOW_VECTOR, @function
TIMER0_OVERFLOW_VECTOR:
	__irq_save__
	XCALL timer0_isr
	__irq_restore__ timer_irq_update_clocksource
	reti

