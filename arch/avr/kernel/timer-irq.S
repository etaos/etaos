/*
 *  ETA/OS - AVR timer handlers
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

#include <asm/io.h>
#include <asm/init.h>
#include <asm/asmdef.h>
#include <asm/irq.h>

/* define standard registers */
__tmp_reg__  = 0
__zero_reg__ = 1

.extern timer0_isr
.extern preempt_schedule_irq

.section .text

.type asm_preempt_timer_irq, @function
asm_preempt_timer_irq:
#ifdef CONFIG_PREEMPT
	irq_return_save
	XCALL preempt_schedule_irq
	irq_return_restore
#endif
	reti

.global TIMER0_OVERFLOW_VECTOR
.type TIMER0_OVERFLOW_VECTOR, @function
TIMER0_OVERFLOW_VECTOR:
	__irq_save__
	XCALL timer0_isr
	__irq_restore__ asm_preempt_timer_irq
	ret

