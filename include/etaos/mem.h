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

#ifndef __MEM_H__
#define __MEM_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>

#ifdef CONFIG_MM_TRACE_OWNER
#include <etaos/thread.h>
#endif

#define MM_MAGIC_BYTE 0x99

struct heap_node {
	struct heap_node *next;
	uint8_t magic;
	
	size_t size;
	unsigned long flags;
#ifdef CONFIG_MM_TRACE_OWNER
	struct thread *owner;
#endif
};

#define MM_ALLOC_FLAG 0

#define MEM __attribute__((malloc))

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

#if defined(CONFIG_STRING) || defined(CONFIG_STRING_MODULE)
#include <etaos/string.h>
static inline void *kzalloc(size_t size)
{
	void *data;

	data = mm_alloc(size);
	if(data)
		memset(data, 0, size);

	return data;
}
#else
static inline void *kzalloc(size_t size)
{
	void *data;
	volatile unsigned char *ptr;

	data = mm_alloc(size);
	if(data) {
		ptr = data;
		do {
			*ptr++ = 0;
		} while(--size);
	}
	
	return data;
}
#endif

#define kmalloc(__s) mm_alloc(__s)
#define kfree(__ptr) mm_kfree(__ptr)

#endif

