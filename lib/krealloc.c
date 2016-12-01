/*
 *  ETA/OS - Realloc
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/mem.h>
#include <etaos/stdlib.h>

/**
 * @brief Expand or shrink an allocated memory region.
 * @param old Memory region that needs shrinking or expansion.
 * @param newsize Desired size.
 * @return The reallocated memory of size \p newsize.
 *
 * All data that is already stored in \p old will be copied to the new buffer
 * by realloc, up to a maximum of \p newsize bytes.
 */
void *realloc(void *old, size_t newsize)
{
	void *new;
	size_t oldsize;

	if((new = kzalloc(newsize)) == NULL)
		return NULL;

	oldsize = mm_node_size(old);

	if(oldsize == newsize) {
		/*
		 * The new block is as big as the old block
		 * therefore there is no further action required
		 * and the input can be returned immediately.
		 */
		kfree(new);
		return old;
	}

	if(oldsize > newsize)
		oldsize = newsize;

	memcpy(new, old, oldsize);
	kfree(old);
	return new;
}

