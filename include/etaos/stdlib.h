/*
 *  Eta/OS - ETA/OS stdlib header
 *  Copyright (C) 2014, 2015   Michel Megens <dev@michelmegens.net>
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

#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <etaos/types.h>
#include <asm/io.h>

extern void srand(uint32_t seed);
extern uint32_t random_m(uint32_t max);
extern uint32_t random(void);
extern uint32_t random_r(uint32_t *sptr, uint32_t max);

extern void *memcpy(void *dst, const void *src, size_t length);
extern int   memcmp(const void *r1, const void *r2, size_t nbytes);

#ifndef __HARVARD__
/**
 * @ingroup libc
 * @def memcpy_P
 * @brief Copy memory from a one location to another.
 * @param x Destination address.
 * @param y Source address.
 * @param z Number of bytes to copy.
 *
 * Not all architectures support the 'P' version of memcpy (i.e. not all
 * architectures can copy memory from program space to data space). If this
 * operation is not supported, this function will redirect to the standard
 * memcpy implementation.
 */
#define memcpy_P(x,y,z) memcpy(x,y,z)
#define strlen_P(c) strlen(c)
#define PSTR(s) (s)
#endif

#endif /* __STDLIB_H__ */

