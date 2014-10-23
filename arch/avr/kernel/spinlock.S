/*
 *  ETA/OS - Spinlocks
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

#include <asm/config.h>
#include <asm/io.h>

.macro EXCH reg1 reg2
	eor \reg1, \reg2
	eor \reg2, \reg1
	eor \reg1, \reg2
.endm

.section .text
.global avr_spin_lock
.func avr_spin_lock
avr_spin_lock:
	/* save clobbers */
	push r30 /* Lower X */
	push r31 /* Higher X */
	push r28 /* Lock value */
	push r27

	mov r30, r24
	mov r31, r25
.L_spin_lock:
	ld r27, Z
	ldi r28, 1
	EXCH r27, r28
	st Z, r27
	tst r28
	brne .L_spin_lock

	/* restore clobbers */
	pop r27
	pop r28
	pop r31
	pop r30
	ret
.endfunc

.global avr_spin_wait
.func avr_spin_wait
avr_spin_wait:
	push r30
	push r31
	push r20

	mov r30, r24
	mov r31, r25
.L_spin_wait:
	ld r20, Z
	tst r20
	brne .L_spin_wait

	pop r20
	pop r31
	pop r30
	ret
.endfunc

.global avr_spin_unlock
.func avr_spin_unlock
avr_spin_unlock:
	/* push clobbers */
	push r30
	push r31

	mov r30, r24
	mov r31, r25
	st Z, __zero_reg__

	pop r31
	pop r30
	ret
.endfunc

