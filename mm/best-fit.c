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

static inline bool bf_select_new_node(struct heap_node *old,
		struct heap_node *new)
{
	if(old) {
		if(old->size > new->size)
			return true;
	}

	return false;
}

MEM void *mm_alloc(size_t size)
{
	void *rv;
	struct heap_node *c, *bf, *prev;

	spin_lock(&mlock);
	c = mm_head;
	prev = NULL;
	bf = NULL;

	while(c) {
		if(c->size == size)
			break;

		if(c->size > size) {
			if(c->size <= size+sizeof(*c)+4)
				break;

			if(bf_select_new_node(bf, c)) {
				bf = c;
				rv = prev;
			}
		}

		prev = c;
		c = c->next;
	}

	if(c == NULL && bf == NULL)
		goto err_l;

	if(c == NULL && bf != NULL) {
		if(bf->size > size+sizeof(*bf)+4)
			mm_split_node(bf, size);
		c = bf;
		prev = rv;
	}

	mm_use_block(c, prev);
	rv = c;
	rv += sizeof(*c);
	return rv;

err_l:
	spin_unlock(&mlock);
	return NULL;
}

int mm_kfree(void *ptr)
{
	return -1;
}

/** @} @} */

