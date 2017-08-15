/*
 *  ETA/OS - Free list heap allocation
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
 * @addtogroup ff
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/stdio.h>
#include <etaos/panic.h>

#include "mm.h"

/**
 * @brief First fit comparator.
 * @param prev Previously selected node.
 * @param current Node to compare.
 * @return Error code.
 * @see raw_mm_heap_alloc
 */
int mm_first_fit_compare(struct heap_node *prev, struct heap_node *current)
{
	return MM_ACCEPT_NODE;
}

/**
 * @brief Allocate a new memory region.
 * @param size Number of bytes to allocate.
 * @return The allocated memory region of size \p size or \p NULL.
 */
MEM void *mm_first_fit_alloc(size_t size)
{
	unsigned long flags;
	void *rv;

	raw_spin_lock_irqsave(&mlock, flags);
	rv = raw_mm_heap_alloc(&mm_free_list, size, &mm_first_fit_compare);
	raw_spin_unlock_irqrestore(&mlock, flags);

	return rv;
}

/** @} */

