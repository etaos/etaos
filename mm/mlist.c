/*
 *  ETA/OS - Memory module
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

/** @file mm/mlist.c */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/spinlock.h>

/**
 * @addtogroup mm
 * @{
 */

DEFINE_SPINLOCK(mlock);

/**
 * @brief Add a block to the mem list.
 * @param start Starting address of the block.
 * @param size Size of the block.
 */
void mm_heap_add_block(void *start, size_t size)
{
	unsigned long flags;

	spin_lock_irqsave(&mlock, flags);
	if(!mm_head) {
		spin_unlock_irqrestore(&mlock, flags);
		return;
	}

	mm_init_node(start, size - sizeof(struct heap_node));
	spin_unlock_irqrestore(&mlock, flags);
	kfree(start+sizeof(struct heap_node));
}

/**
 * @brief Mark a memory block as used.
 * @param node Node to mark as used.
 * @param prev Node precending \p node (i.e. prev->next == node).
 * @note Set \p prev to \p NULL if you are passing the head of the heap.
 */
void mm_use_block(struct heap_node *node, struct heap_node *prev)
{
	set_bit(MM_ALLOC_FLAG, &node->flags);
	if(prev != NULL && node->next != NULL) {
		prev->next = node->next;
		return;
	}

	if(prev == NULL) {
		mm_head = node->next;
		return;
	}

	if(node->next == NULL) {
		prev->next = NULL;
		return;
	}
}

/**
 * @brief calculate how many bytes of memory there are left in the heap.
 * @return Amount of bytes left in the heap.
 */
size_t mm_heap_available(void)
{
	struct heap_node *c;
	size_t total;
	
	_raw_spin_lock(&mlock);
	c = mm_head;
	total = 0;
	
	while(c) {
		total += c->size;
		c = c->next;
	}

	_raw_spin_unlock(&mlock);

#ifdef CONFIG_MMDEBUG
	printf("Mem avail: %u\n", total);
#endif
	return total;
}

/**
 * @brief Merge to nodes.
 * @param a First node.
 * @param b Second node.
 * @return Merged node.
 * @retval \p NULL if the nodes can't merge.
 *
 * The order of the parameters doesn't matter. In other words \p a can be both
 * the lower and higher node.
 */
struct heap_node *mm_merge_node(struct heap_node *a, 
		struct heap_node *b)
{
	void *alpha, *beta;
	if(a->magic != MM_MAGIC_BYTE || b->magic != MM_MAGIC_BYTE) {
#ifdef CONFIG_MM_DEBUG
		fprintf(stderr, "MM magic mismatch in %p or %p\n", a, b);
#endif
		return ERR_PTR;
	}

	alpha = a;
	beta = b;
	if(((alpha + sizeof(*a) + a->size) != beta) &&
			((beta + sizeof(*b) + b->size) != alpha))
		return NULL; /* they ain't connecting.. sir. */

	if(beta + sizeof(*b) + b->size == alpha) {
		/* reversed */
		alpha = a;
		a = b;
		b = alpha;
		alpha = a;
	}

	a->size += b->size + sizeof(*b);
	a->next = b->next;
	return a;
}

/**
 * @brief Split a node in two.
 * @param node Node to split.
 * @param ns New size of \p node.
 *
 * The given node \p node will be split in two. The new side of \p node is
 * given in the \p ns parameter. The new node will, ofcourse be containing
 * the remaining bytes.
 */
void mm_split_node(struct heap_node *node, size_t ns)
{
	struct heap_node *next;
	void *tmp;

	if(node->magic != MM_MAGIC_BYTE) {
#ifdef CONFIG_MM_DEBUG
		fprintf(stderr, "MM magic mismatch in %p\n", node);
#endif
		return;
	}

	tmp = node;
	tmp += ns + sizeof(*node);
	mm_init_node(tmp, node->size - ns - sizeof(*node));
	next = tmp;

	next->next = node->next;
	node->next = next;
	node->size = ns;
}

/**
 * @brief Return a node into the heap.
 * @param block Node to return.
 * @return Error code.
 * @retval 0 If the node was returned succesfully.
 * @retval -1 If the node couldn't be returned into the heap.
 */
int mm_return_node(struct heap_node *block)
{
	struct heap_node *node;

	clear_bit(MM_ALLOC_FLAG, &block->flags);

	if(block < mm_head) {
		block->next = mm_head;
		mm_head = block;
		return 0;
	}

	node = mm_head;
	while(node) {
		if(block > node && block < node->next) {
			block->next = node->next;
			node->next = block;
			return 0;
		}

		if(node->next == NULL) {
			node->next = block;
			block->next = NULL;
			return 0;
		}

		node = node->next;
	}

	return -1;
}

/**
 * @brief Mark a node as used.
 * @param node Node to mark as used.
 * @return Error code.
 * @retval 0 on success.
 * @retval -1 if the given node was invalid.
 */
int mm_use_node(struct heap_node *node)
{
	if(node->magic != MM_MAGIC_BYTE)
		return -1;
	set_bit(MM_ALLOC_FLAG, &node->flags);
	return 0;
}

/**
 * @brief Initialise a new node.
 * @param node Node to initialise.
 * @param size Size of \p node.
 */
void mm_init_node(struct heap_node *node, size_t size)
{
	node->magic = MM_MAGIC_BYTE;
	node->size = size;
	node->next = NULL;
	node->flags = 0UL;
}

/** @} */
