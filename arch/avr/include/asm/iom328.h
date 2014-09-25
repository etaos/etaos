/*
 *  ETA/OS ATmega328 IO defs
 *  Copyright (C) 2012   Michel Megens
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

#ifndef __IOM328_H_
#define __IOM328_H_

#define RAMEND 0x7FF
#define RAMSTART 0x100

#define _VECTORS_SIZE (26*4)

#define AVR_IRQ_FLAG 7
#define AVR_IRQ_BITS (1 << AVR_IRQ_FLAG)

#define AVR_STACK_LOW_ADDR 0x3D
#define AVR_STACK_HI_ADDR 0x3E
#define AVR_STATUS_ADDR 0x3F

#define XJMP jmp
#define XCALL call

#if !defined(__zero_reg__)
    #if defined(CONFIG_AVRTINY)
        #define __zero_reg__ r17
    #else
        #define __zero_reg__ r1
    #endif
#endif

#define cli() __asm__ __volatile__("cli")
#define sei() __asm__ __volatile__("sei")


#define SREG (*((volatile unsigned char*)0x5F))

#ifdef CONFIG_SIMUL_AVR

#define SIMO (*((volatile unsigned char*)0x20))
#define SIMI (*((volatile unsigned char*)0x20))


#define STDOUT_PORT SIMO
#define STDIN_PORT SIMI
#endif /* CONFIG_SIMUL_AVR */

#endif

