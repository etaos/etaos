/*
 *  ETA/OS - Atomic header
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

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

#define ATOMIC_INIT(val) { (val) }

typedef struct {
	volatile int value;
} atomic_t;

typedef struct {
	volatile int64_t value;
} atomic64_t;

#include <asm/atomic.h>

CDECL
static inline void atomic_init(atomic_t *atom)
{
	atom->value = 0;
}

static inline void atomic64_init(atomic64_t *atom)
{
	atom->value = 0LL;
}
CDECL_END
#endif
