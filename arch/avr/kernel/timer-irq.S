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
#include <asm/irq.h>
#include <asm/init.h>

/* define standard registers */
__tmp_reg__  = 0
__zero_reg__ = 1

.extern timer0_isr
.extern rq_update_clock

.section .text

timer_irq_update_clocksource:
#ifndef AVR_22BIT_PC
	push r20
#endif
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r30
	push r31

	push r0
	push r1

	in __tmp_reg__, AVR_STATUS_ADDR
	push __tmp_reg__

	eor __zero_reg__, __zero_reg__

	XCALL rq_update_clock

	pop __tmp_reg__
	out AVR_STATUS_ADDR, __tmp_reg__

	pop r1
	pop r0

	pop r31
	pop r30
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18

	ret

.global TIMER0_OVERFLOW_VECTOR
.type TIMER0_OVERFLOW_VECTOR, @function
TIMER0_OVERFLOW_VECTOR:
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r30
	push r31

	push r0
	push r1

	in __tmp_reg__, AVR_STATUS_ADDR
	push __tmp_reg__

	eor __zero_reg__, __zero_reg__

	XCALL timer0_isr

	pop __tmp_reg__
	out AVR_STATUS_ADDR, __tmp_reg__
	/* restore the stack */

	pop r1
	pop r0

	pop r31
	pop r30
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
#ifndef AVR_22BIT_PC
	pop r20
#endif
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

