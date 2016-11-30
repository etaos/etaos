/*
 *  ETA/OS - Spinlock implementation
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>
#include <etaos/irq.h>

#define SPINLOCK_LOCK_BIT 0

unsigned long __sync_lock_test_and_set_4(volatile unsigned long *lock,
		                                unsigned long value)
{
	unsigned long flags;
	unsigned long rv;

	__sync_synchronize();
	irq_save_and_disable(&flags);
	rv = test_and_set_bit(SPINLOCK_LOCK_BIT, lock);
	irq_restore(&flags);

	return rv;
}

void spinlock_init(spinlock_t *lock)
{
	lock->lock = 0UL;
#ifdef CONFIG_SPINLOCK_DEBUG
	lock->owner = NULL;
	lock->acquire_file = NULL;
	lock->acquire_line = 0;
#endif
}

#ifdef CONFIG_SPINLOCK_DEBUG
void spinlock_acquire(spinlock_t *lock, const char *file, int line)
#else
void spinlock_acquire(spinlock_t *lock)
#endif
{
	volatile unsigned long *lock_ptr;

	lock_ptr = &lock->lock;
	while(__sync_lock_test_and_set(lock_ptr, 1))
		while(*lock_ptr);
}

#ifdef CONFIG_SPINLOCK_DEBUG
void spinlock_release(spinlock_t *lock, const char *file, int line)
#else
void spinlock_release(spinlock_t *lock)
#endif
{
	unsigned long *lock_ptr, flags;

	lock_ptr = &lock->lock;
	__sync_synchronize();
	irq_save_and_disable(&flags);
	clear_bit(SPINLOCK_LOCK_BIT, lock_ptr);
	irq_restore(&flags);
}

