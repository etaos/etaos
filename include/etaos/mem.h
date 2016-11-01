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

/** @file etaos/mem.h */

#ifndef __MEM_H__
#define __MEM_H__

/**
 * @addtogroup mm
 * @{
 */
#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>

#ifdef CONFIG_MM_TRACE_OWNER
#include <etaos/thread.h>
#endif

#define MM_MAGIC_BYTE 0x99

/**
 * @brief Heap node structure
 */
struct heap_node {
	struct heap_node *next; //!< Pointer to the next node.
	uint8_t magic; //!< Verfication byte for the freeing process.
	
	size_t size; //!< Size of the node.
	unsigned long flags; //!< Configuration flags.
#ifdef CONFIG_MM_TRACE_OWNER
	struct thread *owner; //!< Owner of the memory region.
#endif
};

#define MM_ALLOC_FLAG 0 //!< This bit becomes set after allocation

#define MEM __attribute__((malloc))

CDECL
extern struct heap_node *mm_head;
extern spinlock_t mlock;

extern MEM void* mm_alloc(size_t);

extern void mm_init_node(struct heap_node *node, size_t size);
extern int mm_use_node(struct heap_node *node);
extern void mm_split_node(struct heap_node *node, size_t ns);
extern struct heap_node *mm_merge_node(struct heap_node *a, 
		struct heap_node *b);
extern int mm_return_node(struct heap_node *node);
extern void mm_use_block(struct heap_node *node, struct heap_node *prev);
extern int mm_kfree(void *ptr);
extern void mm_heap_add_block(void *start, size_t size);
extern size_t mm_heap_available(void);
extern void mm_init(void *start, size_t size);

extern void kfree(void *);
extern void *kzalloc(size_t);
extern void *kmalloc(size_t);
extern void *kcalloc(size_t, size_t);

CDECL_END

/** @} */
#endif

