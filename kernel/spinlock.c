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

#ifdef CONFIG_SPINLOCK_DEBUG
#include <etaos/stdio.h>
#include <etaos/thread.h>
#include <etaos/mem.h>
#endif

#define LOCK_IDX 0

static uint8_t lock_test_and_set(volatile uint8_t *lock,
		                                uint8_t value)
{
	uint8_t lockval;
	unsigned long flags;

	irq_save_and_disable(&flags);
	barrier();
	lockval = lock[LOCK_IDX];
	lock[LOCK_IDX] = value;
	irq_restore(&flags);
	return lockval;
}

void spinlock_init(spinlock_t *lock)
{
	lock->lock = 0;
#ifdef CONFIG_SPINLOCK_DEBUG
	lock->trace.owner = NULL;
	lock->trace.line = -1;
#endif
}

#ifdef CONFIG_SPINLOCK_DEBUG
void spinlock_acquire(spinlock_t *lock, const char *file, int line)
#else
void spinlock_acquire(spinlock_t *lock)
#endif
{
#ifdef CONFIG_SPINLOCK_DEBUG
#ifdef CONFIG_SCHED
	const char *msg = "Spinlock locked! Lock was previously acquired from: "
		"%s:%i by %s\n";
#else
	const char *msg = "Spinlock locked! Lock was previously acquired from: "
		"%s:%i";
#endif
#endif /* CONFIG_SPINLOCK_DEBUG */
	volatile uint8_t *lock_ptr;

	lock_ptr = &lock->lock;
	while(lock_test_and_set(lock_ptr, 1)) {
#ifdef CONFIG_SPINLOCK_DEBUG
#ifdef CONFIG_SCHED
		printf(msg, lock->trace.file, lock->trace.line,
			lock->trace.owner->name);
#else
		printf(msg, lock->trace.file, lock->trace.line);
#endif
#endif

		while(lock_ptr[LOCK_IDX]);
	}
#ifdef CONFIG_SPINLOCK_DEBUG
	__trace_set(&lock->trace, file, line, false);
#endif /* CONFIG_SPINLOCK_DEBUG */
}

#ifdef CONFIG_SPINLOCK_DEBUG
void spinlock_release(spinlock_t *lock, const char *file, int line)
#else
void spinlock_release(spinlock_t *lock)
#endif
{
	volatile uint8_t *lock_ptr;
	unsigned long flags;

	lock_ptr = &lock->lock;
	irq_save_and_disable(&flags);
	barrier();
#ifdef CONFIG_SPINLOCK_DEBUG
	trace_unset(&lock->trace);
#endif /* CONFIG_SPINLOCK_DEBUG */
	lock_ptr[LOCK_IDX] = 0;
	irq_restore(&flags);
}
