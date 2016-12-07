/*
 *  Eta/OS - AVR5 arch boot
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#include <asm/config.h>
#include <asm/io.h>

.macro vector name
	.if (. - __vectors < _VECTORS_SIZE)
		.weak \name
		.set \name, __bad_interrupt
		XJMP \name
	.endif
.endm

.section .vectors,"ax",@progbits
.global __vectors
__vectors:
	XJMP	__init
	vector	__vector_1
	vector	__vector_2
	vector	__vector_3
	vector	__vector_4
	vector	__vector_5
	vector	__vector_6
	vector	__vector_7
	vector	__vector_8
	vector	__vector_9
	vector	__vector_10
	vector	__vector_11
	vector	__vector_12
	vector	__vector_13
	vector	__vector_14
	vector	__vector_15
	vector	__vector_16
	vector	__vector_17
	vector	__vector_18
	vector	__vector_19
	vector	__vector_20
	vector	__vector_21
	vector	__vector_22
	vector	__vector_23
	vector	__vector_24
	vector	__vector_25
	vector	__vector_26
	vector	__vector_27
	vector	__vector_28
	vector	__vector_29
	vector	__vector_30
	vector	__vector_31
	vector	__vector_32
	vector	__vector_33
	vector	__vector_34
	vector	__vector_35
	vector	__vector_36
	vector	__vector_37
	vector	__vector_38
	vector	__vector_39
	vector	__vector_40
	vector	__vector_41
	vector	__vector_42
	vector	__vector_43
	vector	__vector_44
	vector	__vector_45
	vector	__vector_46
	vector	__vector_47
	vector	__vector_48
	vector	__vector_49
	vector	__vector_50
	vector	__vector_51
	vector	__vector_52
	vector	__vector_53
	vector	__vector_54
	vector	__vector_55
	vector	__vector_56
	vector	__vector_57
	vector	__vector_58
	vector	__vector_59
	vector	__vector_60
	vector	__vector_61
	vector	__vector_62
	vector	__vector_63
	vector	__vector_64
	vector	__vector_65
	vector	__vector_66
	vector	__vector_67
	vector	__vector_68
	vector	__vector_69
	vector	__vector_70
	vector	__vector_71
	vector	__vector_72
	vector	__vector_73
	vector	__vector_74
	vector	__vector_75
	vector	__vector_76
	vector	__vector_77
	vector	__vector_78
	vector	__vector_79
	vector	__vector_80
	vector	__vector_81
	vector	__vector_82
	vector	__vector_83
	vector	__vector_84
	vector	__vector_85
	vector	__vector_86
	vector	__vector_87
	vector	__vector_88
	vector	__vector_89
	vector	__vector_90
	vector	__vector_91
	vector	__vector_92
	vector	__vector_93
	vector	__vector_94
	vector	__vector_95
	vector	__vector_96
	vector	__vector_97
	vector	__vector_98
	vector	__vector_99
	vector	__vector_100
	vector	__vector_101
	vector	__vector_102
	vector	__vector_103
	vector	__vector_104
	vector	__vector_105
	vector	__vector_106
	vector	__vector_107
	vector	__vector_108
	vector	__vector_109
	vector	__vector_110
	vector	__vector_111
	vector	__vector_112
	vector	__vector_113
	vector	__vector_114
	vector	__vector_115
	vector	__vector_116
	vector	__vector_117
	vector	__vector_118
	vector	__vector_119
	vector	__vector_120
	vector	__vector_121
	vector	__vector_122
	vector	__vector_123
	vector	__vector_124
	vector	__vector_125
	vector	__vector_126
	vector	__vector_127

/*
 * Text section which boots up the AVR core.
 */
.text
.global __bad_interrupt
.func __bad_interrupt
__bad_interrupt:
	.weak __vector_default
	.set __vector_default, __vectors
	XJMP __vector_default
.endfunc

/* __init is the first place the IP jumps to after power on */
.section .init0,"ax",@progbits
.weak	__init
__init:
.weak	__heap_end
.set	__heap_end, 0

.section .init2,"ax",@progbits
	clr	__zero_reg__
	out	AVR_STATUS_ADDR, __zero_reg__
	ldi	r28,lo8(init_stack_ptr)
#ifdef __AVR_XMEGA__
	out	AVR_STACK_LOW_ADDR, r28
	ldi	r29,hi8(init_stack_ptr)
	out	AVR_STACK_HI_ADDR, r29
#else
	ldi	r29,hi8(init_stack_ptr)
	out	AVR_STACK_HI_ADDR, r29
	out	AVR_STACK_LOW_ADDR, r28
#endif  /* __AVR_XMEGA__ */

#ifdef AVR_HAVE_EIND
	out	AVR_EIND_ADDR, __zero_reg__
#endif
#ifdef AVR_HAVE_RAMPX
	out	AVR_RAMPX_ADDR, __zero_reg__
#endif
#ifdef AVR_HAVE_RAMPY
	out	AVR_RAMPY_ADDR, __zero_reg__
#endif
#ifdef AVR_HAVE_RAMPZ
	out	AVR_RAMPZ_ADDR, __zero_reg__
#endif
#ifdef AVR_HAVE_RAMPD
	out AVR_RAMPD_ADDR, __zero_reg__
#endif

.section .init9,"ax",@progbits
	XCALL avr_init
	XJMP _exit

.section .fini9,"ax",@progbits
.global _exit
_exit:
	.weak exit
exit:

.section .fini0,"ax",@progbits
	cli
__end_prog:
	XJMP __end_prog

