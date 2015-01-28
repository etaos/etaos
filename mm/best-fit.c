/*
 *  ETA/OS - Best fit allocator
 *  Copyright (C) 2015   Michel Megens
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

/** @file mm/first-fit.c */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>

/**
 * @addtogroup mm
 * @{
 * @addtogroup bf
 * @{
 */

struct heap_node *mm_head;

/**
 * @brief Initialise the heap.
 * @param start Starting point of the heap.
 * @param size Length of the heap.
 */
void mm_init(void *start, size_t size)
{
	mm_head = start;
	mm_init_node(start, size - sizeof(*mm_head));
}

MEM void *mm_alloc(size_t size)
{
	return NULL;
}

int mm_kfree(void *ptr)
{
	return -1;
}

/** @} @} */

