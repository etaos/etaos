/*
 *  ETA/OS - Atomic AVR header
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

#ifndef __AVR_ATOMIC_H__
#error Do not include this file directly, use #include <etaos/atomic.h>
#endif

#ifndef __AVR_ASM_ATOMIC_H__
#define __AVR_ASM_ATOMIC_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

#include <asm/io.h>

static inline void atomic_add(int nr, atomic_t *atom)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	atom->value += nr;
	irq_restore(&flags);
}

static inline void atomic64_add(int64_t nr, atomic64_t *atom)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	atom->value += nr;
	irq_restore(&flags);
}

static inline void atomic_sub(int nr, atomic_t *atom)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	atom->value -= nr;
	irq_restore(&flags);
}

static inline void atomic64_sub(int64_t nr, atomic64_t *atom)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	atom->value -= nr;
	irq_restore(&flags);
}

static inline int atomic_get(atomic_t *atom)
{
	unsigned long flags;
	int value;

	irq_save_and_disable(&flags);
	value = atom->value;
	irq_restore(&flags);
	
	return value;
}

static inline int64_t atomic64_get(atomic64_t *atom)
{
	int64_t value;
	unsigned long flags;

	irq_save_and_disable(&flags);
	value = atom->value;
	irq_restore(&flags);

	return value;
}

#define atomic_inc(atom) atomic_add(1, atom)
#define atomic64_inc(atom) atomic64_add(1LL, atom)

#define atomic_dec(atom) atomic_sub(1, atom)
#define atomic64_dec(atom) atomic64_sub(1, atom)

#endif /* __AVR_ASM_ATOMIC_H__ */

