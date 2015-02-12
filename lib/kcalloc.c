/*
 *  ETA/OS - calloc
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

/**
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/mem.h>

/**
 * @brief Allocate memory for an array.
 * @param num Number elements in the array.
 * @param size Size of a single element.
 * @return A pointer to the allocated space, or a NULL pointer if there is
 *         not a sufficient consecutive memory region.
 */
void *calloc(size_t num, size_t size)
{
	void *rv;

	size = size * num;
	rv = kzalloc(size);
	return rv;
}

/** @} */

