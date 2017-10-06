/*
 *  ETA/OS - Call trace header
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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

#ifndef __TRACE_H__
#define __TRACE_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

#ifdef CONFIG_SCHED
struct thread;
#endif

typedef struct trace_info {
	const char *file;
	int line;
#ifdef CONFIG_SCHED
	struct thread *owner;
#endif
	bool allocated;
} trace_info_t;

#define __trace_set(nfo, fn, ln, ma) trace_info_set(nfo, fn, ln, ma)
#define trace_set(nfo) trace_info_set(nfo, __FILE__, __LINE__, true)
#define trace_unset(nfo) trace_info_invalidate(nfo)

CDECL
extern void trace_info_set(trace_info_t *info, const char *file, int line, bool alloc);
extern void trace_info_invalidate(trace_info_t *info);
CDECL_END

#endif /* end of include guard: __TRACE_H__ */
