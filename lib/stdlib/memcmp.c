/*
 *  ETA/OS - LibC random number generation
 *  Copyright (C) 2015   Michel Megens <dev@bietje.net>
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
#include <etaos/stdlib.h>
#include <etaos/error.h>

/**
 * @brief Compare two memory regions.
 * @param r1 Pointer to region 1.
 * @param r2 Pointer to region 2.
 * @param nbytes Number of bytes to compare.
 * @retval Zero if the two regions are equal for \p nbytes number of bytes.
 * @retval The difference between the first two differing bytes.
 */
int memcmp(const void *r1, const void *r2, size_t nbytes)
{
	const unsigned char *p1, *p2;

	if(nbytes) {
		p1 = r1;
		p2 = r2;

		do {
			if(*p1++ != *p2++)
				return (*--p1 - *--p2);
		} while(--nbytes);
	}

	return -EOK;
}

/** @} */

