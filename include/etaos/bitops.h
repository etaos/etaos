/*
 *  Eta/OS - Bit ops
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

#ifndef __BITOPS_H__
#define __BITOPS_H__

#include <etaos/types.h>
#include <asm/bitops.h>

#define BIT(x) (1 << x)

#ifndef CONFIG_ARCH_TEST_BIT
static inline bool test_bit(unsigned bit, volatile unsigned long *flags)
{
	return 1UL & (flags[bit / BITS_PER_LONG] >> (bit % BITS_PER_LONG));
}
#endif

#ifndef CONFIG_ARCH_TNC
static inline int test_and_clear_bit(unsigned nr, volatile unsigned long *addr)
{
	return 0;
}
#endif

#ifndef CONFIG_ARCH_TNS
static inline int test_and_set_bit(unsigned nr, volatile unsigned long *addr)
{
	return 0;
}
#endif

#ifndef CONFIG_ARCH_SET_BIT
static inline void set_bit(unsigned nr, volatile unsigned long *addr)
{
	return 0;
}
#endif

#ifndef CONFIG_ARCH_CLEAR_BIT
static inline void clear_bit(unsigned nr, volatile unsigned long *addr)
{
	return 0;
}
#endif

#endif
