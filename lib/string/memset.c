/*
 *  ETA/OS - strchr
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

/**
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/string.h>

/**
 * @brief Fill a block of memory.
 * @param dst Memory region to fill.
 * @param c Filler character.
 * @param n Length \p dst.
 * @return The memory regeion \p dst.
 */
void *memset(void *dst, int c, size_t n)
{
	volatile unsigned char *data;
	size_t idx;

	if((data = dst) == NULL)
		return NULL;

	for(idx = 0; idx < n; idx++)
		data[idx] = (unsigned char)c;

	return dst;
}

/** @} */

