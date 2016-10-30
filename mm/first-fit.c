/*
 *  ETA/OS - Memory module
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

/** @file mm/first-fit.c */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>

/**
 * @addtogroup ff
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

/**
 * @brief Allocated a new memory region.
 * @param size Length of the wanted region.
 *
 * If a memory region which is exactly equal to or at most 4 bytes larger than
 * size, this region is selected. If a free region is found which is 4 bytes
 * larger than size, the region is split in two.
 */
MEM void *mm_alloc(size_t size)
{
	void *rval;
	struct heap_node *c, *prev;
	unsigned long flags;

	spin_lock_irqsave(&mlock, flags);
	c = mm_head;
	prev = NULL;

	while(c) {
		if(c->size == size)
			break;

		if(c->size > size) {
			if(c->size < size+sizeof(*c)+4)
				break;
			mm_split_node(c, size);
			break;
		}
		prev = c;
		c = c->next;
	}

	if(c == NULL) {
		rval = NULL;
		goto err_l;
	}

	mm_use_block(c, prev);
	rval = c;
	rval += sizeof(*c);

err_l:
	spin_unlock_irqrestore(&mlock, flags);
	return rval;
}

/**
 * @brief Free an allocated memory region.
 * @param ptr Memory region to free.
 * @return Error code.
 * @retval 0 Region succesfully free'd.
 * @retval -1 Given region is not located within the heap.
 */
int mm_kfree(void *ptr)
{
	struct heap_node *node, *c;
	unsigned long flags;
	int err = -1;

	spin_lock_irqsave(&mlock, flags);
	node = ptr - sizeof(*node);
	/*if(!test_bit(MM_ALLOC_FLAG, &node->flags))
		goto err_l;*/

	if(node->magic != MM_MAGIC_BYTE)
		goto err_l;

	c = mm_head;
	mm_return_node(node);

	while(c) {
		if(((void*)c) + c->size + sizeof(*c) == node || ((void*)node) +
				node->size + sizeof(*node) == c) {
			node = mm_merge_node(c, node);
			if(node == NULL)
				goto err_l;
			else
				c = node;
		}
		c = c->next;
	}

	err = 0;

err_l:
	spin_unlock_irqrestore(&mlock, flags);
	return err;
}

/**
 * @}
 * @}
 */
