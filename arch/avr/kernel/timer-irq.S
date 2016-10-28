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

#include <asm/asmdef.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/init.h>

.extern timer0_isr
.extern rq_update_clock

.section .text

timer_irq_update_clocksource:
	irq_return_save
	XCALL rq_update_clock
	irq_return_restore

	ret

.global TIMER0_OVERFLOW_VECTOR
.type TIMER0_OVERFLOW_VECTOR, @function
TIMER0_OVERFLOW_VECTOR:
	__irq_save__
	XCALL timer0_isr
	__irq_restore__

	/* The registers r18:r20 are popped of by timer_irq_update_clocksource */
	/*
	pop r19
	pop r18
	*/
#ifdef AVR_22BIT_PC
	ldi r20, pm_hh8(timer_irq_update_clocksource)
#endif
	ldi r19, pm_hi8(timer_irq_update_clocksource)
	ldi r18, pm_lo8(timer_irq_update_clocksource)

	push r18
	push r19
#ifdef AVR_22BIT_PC
	push r20
#endif
	reti

