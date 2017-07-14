/*
 *  ETA/OS - XOR linked list
 *  Copyright (C) 2017  Michel Megens <dev@bietje.net>
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
#include <etaos/xorlist.h>
#include <etaos/error.h>

static inline void *xlist_create_pointer(size_t a, size_t b)
{
	return (void*)(a ^ b);
}

int xlist_add(struct xorlist_head *prev, struct xorlist_head *curr,
		struct xorlist_head *node)
{
	size_t uprev, ucurr, unode, unext, unext_next;
	struct xorlist_head *next;

	uprev = (size_t)prev;
	ucurr = (size_t)curr;
	unode = (size_t)node;
	next = xlist_next(prev, curr);
	unext = (size_t)next;

	unext_next = (next) ? (size_t)next->ptr ^ ucurr : ucurr;

	if(!next) {
		curr->ptr = xlist_create_pointer(uprev, unode);
		node->ptr = curr;
		return -EOK;
	}

	curr->ptr = xlist_create_pointer(uprev, unode);
	node->ptr = xlist_create_pointer(ucurr, unext);
	next->ptr = unext_next ? xlist_create_pointer(unode, unext_next) :
		xlist_create_pointer(unode, 0);

	return -EOK;
}

int xlist_remove(struct xorlist_head *prev, struct xorlist_head *node)
{
	struct xorlist_head *next;
	size_t uprev_prev, unext, uprev, unode, unext_next;

	if(!node)
		return -EINVAL;

	uprev = (size_t)prev;
	unode = (size_t)node;
	next = xlist_next(prev, node);
	unext = (size_t)next;
	uprev_prev = prev ? (size_t)xlist_prev(prev, node) : 0;
	unext_next = next ? (size_t)xlist_create_pointer((size_t)next->ptr,
			unode) : 0;

	if(prev)
		prev->ptr = xlist_create_pointer(uprev_prev, unext);

	if(next)
		next->ptr = xlist_create_pointer(unext_next, uprev);

	node->ptr = NULL;
	return -EINVAL;
}

struct xorlist_head *xlist_next(struct xorlist_head *prev,
		struct xorlist_head *cur)
{
	size_t uprev, ucur;

	if(!cur)
		return NULL;

	uprev = (size_t)prev;
	ucur = (size_t)cur->ptr;

	return xlist_create_pointer(uprev, ucur);

}

struct xorlist_head *xlist_prev(struct xorlist_head *this,
		struct xorlist_head *next)
{
	return (struct xorlist_head*)
		(this ?
		 xlist_create_pointer((size_t)this->ptr, (size_t)next) : NULL);
}

