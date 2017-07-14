/*
 *  ETA/OS - LibC memchr()
 *  Copyright (C) 2004 by Egnite Software GmbH
 *  Copyright (C) 2015  Michel Megens <dev@bietje.net>
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

/**
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/string.h>

/**
 * @brief Find the first occurance of a byte.
 * @param s Memory region to search.
 * @param c Byte to look for.
 * @param n Length of \p s.
 * @return A pointer, within \p s, to the located byte, or NULL if nothing
 *         is found.
 */
void *memchr(const void *s, int c, size_t n)
{
	const char *p;

	if(!n)
		return NULL;

	p = s;
	do {
		if(*p++ == (unsigned char)c)
			return (void*) (p - 1);
	} while(--n);

	return NULL;
}

/** @} */

