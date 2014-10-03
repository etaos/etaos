/*
 *  ETA/OS - Linked List API
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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

#ifndef __LIST_H__
#define __LIST_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

static inline void list_head_init(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *lnew,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = lnew;
	lnew->next = next;
	lnew->prev = prev;
	prev->next = lnew;
}

static inline void __list_del(struct list_head *prev,
			      struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

static inline int list_empty(const struct list_head *head)
{
	return (head->next == head);
}

static inline int list_is_last(const struct list_head *list,
				const struct list_head *head)
{
	return list->next == head;
}

#define list_for_each(pos, head) \
		for(pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
		for(pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_safe(pos, n, head) \
		for(pos = (head)->next, n = pos->next; pos != (head); \
				pos = n, n = pos->next)

#define list_for_each_prev_safe(pos, n, head) \
		for(pos = (head)->prev, n = pos->prev; pos != (head); \
				pos = n, n = pos->prev)

#define list_add(___n, ___h) __list_add(___n, ___h, (___h)->next)
#define list_add_tail(__n, __h) __list_add(__n, (__h)->prev, __h)
#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#endif

