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

/**
 * @addtogroup xorlist
 * @{
 */

#ifndef __XORLIST_H__
#define __XORLIST_H__

#include <etaos/kernel.h>

/**
 * @brief XOR list structure.
 */
struct xorlist_head {
	struct xorlist_head *ptr; //!< List pointer.
};

CDECL
extern int xlist_add(struct xorlist_head *prev,
		     struct xorlist_head *curr,
		     struct xorlist_head *node);
extern int xlist_remove(struct xorlist_head *prev, struct xorlist_head *node);
extern struct xorlist_head *xlist_next(struct xorlist_head *prev,
		struct xorlist_head *cur);
extern struct xorlist_head *xlist_prev(struct xorlist_head *this,
		struct xorlist_head *next);

CDECL_END

/**
 * @brief Iterate of a xlist.
 * @param p Prev pointer.
 * @param h List head.
 * @param c Carriage pointer.
 * @param t Temporary variable.
 */
#define xlist_for_each(p, h, c, t) \
	for(c = h; c; t = c, c = xlist_next(p, c), p = t)

/**
 * @brief Iterate of a xlist safely.
 * @param p Prev pointer.
 * @param h List head.
 * @param c Carriage pointer.
 * @param t Temporary variable.
 * @param s Temporary variable.
 */
#define xlist_for_each_safe(p, h, c, t, s) \
	for(c = h, s = xlist_next(p, h); c; t = c, c = s, \
			s = xlist_next(t, c), p = t)

#endif /* __XORLIST_H__ */

/** @} */

