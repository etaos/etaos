/*
 *  ETA/OS - Python header
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
 *  Copyright (C) 2017   Dean Hall
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

#ifndef __PM_EMPTY_PLATFORM_DEFS_H__
#define __PM_EMPTY_PLATFORM_DEFS_H__

/**
 * \file
 * \brief Empty platform-specific definitions
 *
 * This file #defines as blank any undefined platform-specific
 * definitions.
 */

/**
 * Define a processor-specific specifier for use in declaring the heap.
 * If not defined, make it empty.
 * See <code>pmHeap</code> in heap.c for its use, which is:<br>
 * <code>static PmHeap_t pmHeap PM_PLAT_HEAP_ATTR;</code>
 */
#if !defined(PM_PLAT_HEAP_ATTR) || defined(__DOXYGEN__)
#define PM_PLAT_HEAP_ATTR
#endif

#endif				/* __PM_EMPTY_PLATFORM_DEFS_H__ */
