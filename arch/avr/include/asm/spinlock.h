/*
 *  ETA/OS - Spinlock header
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

/**
 * @file avr/include/asm/spinlock.h AVR spinlock header
 */

#if !defined(__MUTEX_H__) && !defined(__SPINLOCK_H__)
#error Do not include <asm/spinlock.h> directly. \
	Use <etaos/mutex.h> or <etaos/spinlock.h>!
#endif

#ifndef __AVR_SPINLOCK_H__
#define __AVR_SPINLOCK_H__

typedef struct spinlock {
	uint8_t lock;
} spinlock_t;

CDECL

extern void avr_spin_lock(unsigned char *);
extern void avr_spin_unlock(unsigned char*);
extern void avr_spin_wait(unsigned char *);

static inline void spin_lock_init(spinlock_t *lock)
{
	lock->lock = 0;
}

/**
 * @ingroup archAPI
 * @brief Arch implementation of the spin wait.
 * @param spin Spin to wait for.
 */
static inline void arch_spin_wait(spinlock_t *spin)
{
	avr_spin_wait((unsigned char*)&spin->lock);
}

/**
 * @ingroup archAPI
 * @brief Raw version of spin_lock.
 * @param spin Spin lock to lock.
 */
static inline void arch_spin_lock(spinlock_t *spin)
{
	avr_spin_lock((unsigned char*)&spin->lock);
}

/**
 * @ingroup archAPI
 * @brief Raw version of spin_unlock
 * @param spin Spin lock to unlock.
 */
static inline void arch_spin_unlock(spinlock_t *spin)
{
	avr_spin_unlock((unsigned char*)&spin->lock);
}
CDECL_END

#endif

