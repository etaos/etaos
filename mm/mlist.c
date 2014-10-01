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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/mutex.h>

DEFINE_MUTEX(mlock);

void mm_heap_add_block(void *start, size_t size)
{
	mutex_lock(&mlock);
	if(!mm_head)
		return;

	mm_init_node(start, size - sizeof(struct heap_node));
	mutex_unlock(&mlock);
	kfree(start+sizeof(struct heap_node));
}

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

size_t mm_heap_available(void)
{
	struct heap_node *c;
	size_t total;
	
	mutex_lock(&mlock);
	c = mm_head;
	total = 0;
	
	while(c) {
		total += c->size;
		c = c->next;
	}

	mutex_unlock(&mlock);

#ifdef CONFIG_MMDEBUG
	printf("Mem avail: %u\n", total);
#endif
	return total;
}

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

int mm_return_node(struct heap_node *node)
{
	struct heap_node *carriage;

	clear_bit(MM_ALLOC_FLAG, &node->flags);

	if(node < mm_head) {
		node->next = mm_head;
		mm_head = node;
		return 0;
	}

	carriage = mm_head;
	while(carriage) {
		if(node > carriage && node < carriage->next) {
			node->next = carriage->next;
			carriage->next = node;
		}

		if(carriage->next == NULL) {
			carriage->next = node;
			node->next = NULL;
			return 0;
		}

		carriage = carriage->next;
	}

	return -1;
}

int mm_use_node(struct heap_node *node)
{
	if(node->magic != MM_MAGIC_BYTE)
		return -1;
	set_bit(MM_ALLOC_FLAG, &node->flags);
	return 0;
}

void mm_init_node(struct heap_node *node, size_t size)
{
	node->magic = MM_MAGIC_BYTE;
	node->size = size;
	node->next = NULL;
	node->flags = 0UL;
}

