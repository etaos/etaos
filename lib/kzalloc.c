/*
 *  ETA/OS - Zero alloc
 *  Copyright (C) 2014   Michel Megens
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
#include <etaos/mem.h>

#if defined(CONFIG_STRING) || defined(CONFIG_STRING_MODULE)
#include <etaos/string.h>
/**
 * @brief Allocate a memory regeion.
 * @param size Size of the region.
 *
 * The content of the allocated region will be set to 0.
 */
void *kzalloc(size_t size)
{
	void *data;

	data = mm_alloc(size);
	if(data)
		memset(data, 0, size);

	return data;
}
#else
void *kzalloc(size_t size)
{
	void *data;
	volatile unsigned char *ptr;

	data = mm_alloc(size);
	if(data) {
		ptr = data;
		do {
			*ptr++ = 0;
		} while(--size);
	}
	
	return data;
}
#endif

/** @} */

