/*
 *  ETA/OS - Free list heap allocation
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
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/spinlock.h>
#include <etaos/panic.h>

#include <asm/pgm.h>

static struct heap_node *mm_free_list = NULL;
static DEFINE_SPINLOCK(mlock);

typedef size_t uintptr_t;

#ifdef CONFIG_MM_GUARD
#define MM_GUARD_PATTERN 0xDEADBEEF
#define MM_GUARD_BYTES   sizeof(MM_GUARD_PATTERN)
#else
#define MM_GUARD_BYTES   0
#endif

#define MM_ALIGNMENT          1
#define MM_BOTTOM_ALIGN(s)    ((s) & ~(MM_ALIGNMENT - 1))
#define MM_TOP_ALIGN(s)	      MM_BOTTOM_ALIGN((s + (MM_ALIGNMENT - 1)))

#define HEAP_OVERHEAD (sizeof(struct heap_node) - sizeof(struct heap_node*))
#define HEAP_MIN (sizeof(struct heap_node) + (2 * MM_GUARD_BYTES))

void mm_init(void)
{
}

static void *mm_prep_user_area(struct heap_node *node)
{
    int *tp = (int *) (uintptr_t) &node->next;
#ifdef CONFIG_MM_GUARD
    size_t off = (node->size - HEAP_OVERHEAD) / sizeof(int) - 2;

    *tp++ = MM_GUARD_PATTERN;
    *(tp + off) = MM_GUARD_PATTERN;
#endif
    return tp;
}

static int mm_validate_user_area(struct heap_node *node)
{
	return -EOK;
}

static void *raw_mm_heap_alloc(struct heap_node **root, size_t size)
{
	struct heap_node *node,
			 **npp,
			 *fit = NULL,
			 **fpp = NULL;
	size_t need;

	/* Determine the actual allocation size */
	need = size + HEAP_OVERHEAD + 2 * MM_GUARD_BYTES;
	if(need < sizeof(struct heap_node))
		need = sizeof(struct heap_node);

	need = MM_TOP_ALIGN(need);

	/* Now we need to find the best fitting node */
	node = *root;
	npp = root;
	while(node) {
		if(node->size >= need) {
			/* This node fits! */
			if(fit == NULL || fit->size > node->size) {
				/*
				 * This node fits better than the one
				 * we found previously
				 */
				fit = node;
				fpp = npp;
				if(node->size == need)
					break;
			}
		}

		npp = &node->next;
		node = node->next;
	}

	if(fit) {
		if(fit->size - need >= HEAP_MIN) {
			/*
			 * Node is large enough to fit another node,
			 * so we split the node in two
			 */
			node = (struct heap_node *)((uintptr_t) fit + need);
			node->size = fit->size - need;
			node->next = fit->next;
			fit->size = need;
			*fpp = node;
		} else {
			/*
			 * Cannot split the node, so we use the full one
			 * instead.
			 */
			*fpp = fit->next;
		}

		fit = mm_prep_user_area(fit);
	}

	return fit;
}

#ifdef CONFIG_MM_DEBUG
static int raw_mm_heap_free(struct heap_node **root, void *block,
		const char *file, int line)
#else
static int raw_mm_heap_free(struct heap_node **root, void *block)
#endif
{
	struct heap_node *node,
			 **npp,
			 *fnode;

	if(!block)
		return -EINVAL;

	fnode = (struct heap_node*)
		((uintptr_t)block - (HEAP_OVERHEAD + MM_GUARD_BYTES));

	if(mm_validate_user_area(fnode))
		return -EINVAL;

	node = *root;
	npp = root;
	while(node) {
		if((uintptr_t)node + node->size == (uintptr_t)fnode) {
			/* Node fits right in front */
			node->size += fnode->size;

			/* Check if another node is directly following,
			 * if so, merge it aswell
			 */
			if(((uintptr_t)node + node->size) ==
					(uintptr_t)node->next) {
				node->size += node->next->size;
				node->next = node->next->next;
			}
			break;
		}

		if((uintptr_t)node > (uintptr_t)fnode) {
			/* We are past the walker address now */
			*npp = fnode;
			if(((uintptr_t)fnode + fnode->size) == (uintptr_t)node) {
				fnode->size += node->size;
				fnode->next = node->next;
			} else {
				fnode->next = node;
			}

			break;
		}

		/*
		 * If we are within another node address, somebody probably
		 * has tried to free up an invalid address, or free'd a
		 * node twice.
		 */
		if(((uintptr_t)node + node->size) > (uintptr_t)fnode) {
#ifdef CONFIG_MM_DEBUG
			panic("Trying to release free heap memory [%p] from %s:%i\n",
					block, file, line);
#else
			panic_P(PSTR("Trying to release free heap memory [%p]\n"),
					block);
#endif
			return -EINVAL;
		}

		npp = &node->next;
		node = node->next;
	}

	if(!node) {
		fnode->next = node;
		*npp = fnode;
	}

	return -EOK;
}

MEM void *mm_alloc(size_t size)
{
	unsigned long flags;
	void *rv;

	raw_spin_lock_irqsave(&mlock, flags);
	rv = raw_mm_heap_alloc(&mm_free_list, size);
	raw_spin_unlock_irqrestore(&mlock, flags);

	return rv;
}

#ifdef CONFIG_MM_DEBUG
int mm_free(void *block, const char *file, int line)
#else
int mm_free(void *block)
#endif
{
	unsigned long flags;
	int rv;

	raw_spin_lock_irqsave(&mlock, flags);
#ifdef CONFIG_MM_DEBUG
	rv = raw_mm_heap_free(&mm_free_list, block, file, line);
#else
	rv = raw_mm_heap_free(&mm_free_list, block);
#endif
	raw_spin_unlock_irqrestore(&mlock, flags);

	return rv;
}

void raw_mm_heap_add_block(void *addr, size_t size)
{
	struct heap_node *node = (struct heap_node*)MM_TOP_ALIGN((uintptr_t)addr);

	node->size = MM_BOTTOM_ALIGN(size - ((uintptr_t)node - (uintptr_t)addr));
#ifdef CONFIG_MM_DEBUG
	raw_mm_heap_free(&mm_free_list, mm_prep_user_area(node), __FILE__, __LINE__);
#else
	raw_mm_heap_free(&mm_free_list, mm_prep_user_area(node));
#endif
}

void mm_heap_add_block(void *addr, size_t size)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&mlock, flags);
	raw_mm_heap_add_block(addr, size);
	raw_spin_unlock_irqrestore(&mlock, flags);
}

static size_t raw_mm_heap_available(struct heap_node **root)
{
	size_t rv = 0UL;
	struct heap_node *node;

	for(node = *root; node; node = node->next) {
		rv += node->size - HEAP_OVERHEAD;
	}

	return rv;
}

size_t mm_heap_available(void)
{
	size_t rv;
	unsigned long flags;

	raw_spin_lock_irqsave(&mlock, flags);
	rv = raw_mm_heap_available(&mm_free_list);
	raw_spin_unlock_irqrestore(&mlock, flags);

	return rv;
}

