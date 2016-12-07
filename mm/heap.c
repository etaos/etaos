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

/**
 * @addtogroup bf
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/spinlock.h>
#include <etaos/stdio.h>
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

/**
 * @brief Initialise the memory allocator.
 */
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
#ifdef CONFIG_MM_DESTRUCTIVE_ALLOC
	} else {
		panic("NO MEMORY!\n");
	}
#else
	}
#endif

	return fit;
}

static inline struct heap_node *mm_region_to_node(void *ptr)
{
	return (struct heap_node *)
		((uintptr_t)ptr - (HEAP_OVERHEAD + MM_GUARD_BYTES));
}

/**
 * @brief Get the size of a memory region.
 * @param ptr Memory region to get the size of.
 * @return The size of the memory region pointed to by \p ptr.
 * @note This function will acquire the memory lock.
 */
size_t mm_node_size(void *ptr)
{
	struct heap_node *node;
	size_t size;
	unsigned long flags;

	raw_spin_lock_irqsave(&mlock, flags);
	node = mm_region_to_node(ptr);
	size = node->size;
	raw_spin_unlock_irqrestore(&mlock, flags);

	return size;
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

	fnode = mm_region_to_node(block);

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
			fprintf(stderr, "Trying to release free heap memory "
					"[%p] from %s:%i\n",
					block, file, line);
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

/**
 * @brief Allocate a new memory region.
 * @param size Number of bytes to allocate.
 * @return The allocated memory region of size \p size or \p NULL.
 */
MEM void *mm_alloc(size_t size)
{
	unsigned long flags;
	void *rv;

	raw_spin_lock_irqsave(&mlock, flags);
	rv = raw_mm_heap_alloc(&mm_free_list, size);
	raw_spin_unlock_irqrestore(&mlock, flags);

	return rv;
}

/**
 * @brief Free a previously allocated memory region.
 * @param block Memory block to free.
 * @return An error code.
 * @retval -EOK on success.
 * @retval -EINVAL on failure.
 */
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

/**
 * @brief Add a new memory region to the allocator.
 * @param addr Start of the memory region.
 * @param size Seize of the region pointed to by \p addr.
 * @note This function does not acquire the global memory lock.
 */
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

/**
 * @brief Add a new memory region to the allocator.
 * @param addr Start of the memory region.
 * @param size Seize of the region pointed to by \p addr.
 * @note This function acquires the global memory lock.
 */
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

/**
 * @brief Get the total number of bytes available in the allocator.
 * @return The number of free bytes available in the allocator.
 */
size_t mm_heap_available(void)
{
	size_t rv;
	unsigned long flags;

	raw_spin_lock_irqsave(&mlock, flags);
	rv = raw_mm_heap_available(&mm_free_list);
	raw_spin_unlock_irqrestore(&mlock, flags);

	return rv;
}

/** @} */

