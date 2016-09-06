/*
 *  ETA/OS - AVR bitops
 *  Copyright (C) 2014, 2015   Michel Megens <dev@bietje.net>
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

/**
 * @file avr/include/asm/bitops.h
 */

#ifndef __AVR_BITOPS_H_
#define __AVR_BITOPS_H_

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>

#define BITS_PER_LONG 32UL
#define BITS_PER_BYTE  8UL

CDECL
/**
 * @brief Clear a bit in register.
 * @param nr Bit to clear.
 * @param flags Register that contains \p nr.
 */
static inline void clear_bit(unsigned nr, volatile void *flags)
{
	volatile unsigned char *p = ((volatile unsigned char*)flags) + 
		(nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	volatile unsigned char tmp;

	__asm__ __volatile__(
			"ld %0, %a2"	"\n\t"
			"com %3"	"\n\t"
			"and %0, %3"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&r" (tmp), "=z" (p)
			: "z" (p), "r" (msk)
			: "memory"
			);
}

/**
 * @brief Set a bit in a register.
 * @param nr Bit to set.
 * @param addr Register containing \p nr.
 */
static inline void set_bit(unsigned nr, volatile void *addr)
{
	volatile unsigned char *p = ((volatile unsigned char*)addr) + 
		(nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	volatile unsigned char tmp;

	__asm__ __volatile__(
			"ld %0, %a2"	"\n\t"
			"or %0, %3"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&r" (tmp), "=z" (p)
			: "z" (p), "r" (msk)
			: "memory"
			);
}

/**
 * @brief Return the value of bit \p nr.
 * @param nr Bit number to test.
 * @param addr Register that contains \p nr.
 * @return The value of bit \p nr.
 */
static inline int test_bit(unsigned nr, volatile void *addr)
{
	volatile unsigned char *p = ((unsigned char *)addr) + 
		(nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	volatile unsigned char tmp;

	__asm__ __volatile__(
			"ld %0, %a1"	"\n\t"
			"and %0, %2"	"\n\t"
			: "=&r" (tmp)
			: "z" (p), "d" (msk)
			: "memory"
			);

	return tmp != 0;
}

/**
 * @brief Clear a bit in register and return the value of \p nr before clearing.
 * @param nr Bit to clear.
 * @param addr Register that contains \p nr.
 * @return Initial bit value of bit \p nr.
 */
static inline int test_and_clear_bit(unsigned nr, volatile void *addr)
{
	volatile unsigned char *p = ((unsigned char*)addr) + 
		(nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	volatile unsigned char tmp, old;

	__asm__ __volatile__(
			"ld %0, %a3"	"\n\t"
			"mov %2, %0"	"\n\t"
			"and %2, %4"	"\n\t"
			"com %4"	"\n\t"
			"and %0, %4"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&r" (tmp), "=z" (p), "=&r" (old)
			: "z" (p), "d" (msk)
			: "memory"
			);
	return old != 0;
}

/**
 * @brief Set a bit in register and return the value of \p nr before setting it.
 * @param nr Bit to set.
 * @param addr Register that contains \p nr.
 * @return Initial bit value of bit \p nr.
 */
static inline int test_and_set_bit(unsigned nr, volatile void *addr)
{
	volatile unsigned char *p = ((unsigned char*)addr) + 
		(nr / BITS_PER_BYTE);
	unsigned char msk = 1UL << (nr % BITS_PER_BYTE);
	volatile unsigned char tmp, old;

	__asm__ __volatile__(
			"ld %0, %a3"	"\n\t"
			"mov %2, %0"	"\n\t"
			"and %2, %4"	"\n\t"
			"or %0, %4"	"\n\t"
			"st %a1, %0"	"\n\t"
			: "=&r" (tmp), "=z" (p), "=&r" (old)
			: "z" (p), "d" (msk)
			: "memory"
			);
	return old != 0;
}
CDECL_END

#endif

