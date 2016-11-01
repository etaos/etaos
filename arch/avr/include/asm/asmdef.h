/*
 *  ETA/OS - Assembly definitions
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

#ifndef __ASM_DEF_H__
#define __ASM_DEF_H__

#include <asm/io.h>
#include <asm/config.h>

#ifdef __ASSEMBLER__
/* define standard registers */
__tmp_reg__  = 0
__zero_reg__ = 1

/* Save registers and status register in IRQ return routine */
.macro irq_return_save
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

#ifdef AVR_HAVE_RAMPZ
	in __tmp_reg__, AVR_RAMPZ_ADDR
	push __tmp_reg__
#endif

	in __tmp_reg__, AVR_STATUS_ADDR
	push __tmp_reg__

	eor __zero_reg__, __zero_reg__
.endm

/* Restore registers and status register in IRQ return routine */
.macro irq_return_restore
	pop __tmp_reg__
	out AVR_STATUS_ADDR, __tmp_reg__

#ifdef AVR_HAVE_RAMPZ
	pop __tmp_reg__
	out AVR_RAMPZ_ADDR, __tmp_reg__
#endif

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
.endm

/* Save state before entering an IRQ */
.macro __irq_save__
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

#ifdef AVR_HAVE_RAMPZ
	in __tmp_reg__, AVR_RAMPZ_ADDR
	push __tmp_reg__
#endif

	in __tmp_reg__, AVR_STATUS_ADDR
	push __tmp_reg__

	eor __zero_reg__, __zero_reg__
.endm

/* Restore state before leaving the IRQ */
.macro __irq_restore__ ref
	pop __tmp_reg__
	out AVR_STATUS_ADDR, __tmp_reg__

#ifdef AVR_HAVE_RAMPZ
	pop __tmp_reg__
	out AVR_RAMPZ_ADDR, __tmp_reg__
#endif

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
#ifdef CONFIG_PREEMPT_FULL
#ifndef AVR_22BIT_PC
	pop r20
#else /* #ifdef AVR_22BIT_PC */
	ldi r20, pm_hh8(\ref)
#endif /* AVR_22BIT_PC */
	ldi r19, pm_hi8(\ref)
	ldi r18, pm_lo8(\ref)

	push r18
	push r19
#ifdef AVR_22BIT_PC
	push r20
#endif
#else
	pop r20
	pop r19
	pop r18
#endif
.endm

#endif /* __ASSEMBLER__ */
#endif /* __ASM_DEF_H__ */

