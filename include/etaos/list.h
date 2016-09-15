/*
 *  ETA/OS - Linked List API
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

/**
 * @file etaos/list.h
 */

#ifndef __LIST_H__
#define __LIST_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

/**
 * @brief Static list initialiser.
 * @param name Name of the list which has to be initialised.
 */
#define STATIC_INIT_LIST_HEAD(name) { &(name), &(name) }
CDECL

/**
 * @brief Dynamic list initialiser.
 * @param list List which has to be initialised.
 */
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

/**
 * @brief Delete an entry from a linked list.
 * @param entry Entry which has to be deleted.
 */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

/**
 * @brief Check if a list is empty.
 * @param head List head to check.
 * @return True or false based on whether the list is empty or not.
 * @retval true if the list is empty.
 * @retval false if the list is not empty.
 */
static inline int list_empty(const struct list_head *head)
{
	return (head->next == head);
}

/**
 * @brief Check if an entry is the last entry of a list.
 * @param list List entry to check.
 * @param head List head.
 * @return True or false based on whether the entry is the last entry or not.
 */
static inline int list_is_last(const struct list_head *list,
				const struct list_head *head)
{
	return list->next == head;
}

CDECL_END

/**
 * @brief Loop through a list.
 * @param pos Carriage pointer.
 * @param head List head.
 */
#define list_for_each(pos, head) \
		for(pos = (head)->next; pos != (head); pos = pos->next)

/**
 * @brief Loop reversed through a list.
 * @param pos Carriage pointer.
 * @param head Head pointer.
 */
#define list_for_each_prev(pos, head) \
		for(pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * @brief Loop safely through a list.
 * @param pos Carriage pointer.
 * @param n Temporary value.
 * @param head List head.
 *
 * Safe means that the current item (\p pos) can be deleted.
 */
#define list_for_each_safe(pos, n, head) \
		for(pos = (head)->next, n = pos->next; pos != (head); \
				pos = n, n = pos->next)

/**
 * @brief Loop backwards, in a safe manner through a list.
 * @param pos Carriage pointer.
 * @param n Temporary value.
 * @param head List head.
 * @see list_for_each_safe
 */
#define list_for_each_prev_safe(pos, n, head) \
		for(pos = (head)->prev, n = pos->prev; pos != (head); \
				pos = n, n = pos->prev)

/**
 * @brief Add a new list node.
 * @param ___n Node to add.
 * @param ___h List head.
 */
#define list_add(___n, ___h) __list_add(___n, ___h, (___h)->next)
/**
 * @brief Add a new entry to the tail.
 * @param __n New node.
 * @param __h List head.
 */
#define list_add_tail(__n, __h) __list_add(__n, (__h)->prev, __h)
/**
 * @brief Get the list container structure.
 * @param ptr Entry pointer.
 * @param type Container type.
 * @param member Container member name.
 */
#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#endif

