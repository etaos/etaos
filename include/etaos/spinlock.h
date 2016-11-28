/*
 *  ETA/OS - Spinlock
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
 * @file etaos/spinlock.h Spin lock header
 */

/**
 * @addtogroup kernel
 * @{
 */

#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/preempt.h>

#include <asm/irq.h>

typedef struct spinlock {
	unsigned long lock;
#ifdef CONFIG_SPINLOCK_DEBUG
	struct thread *owner;
	char *acquire_file;
	int   acquire_line;
#endif
} spinlock_t;

#define DEFINE_SPINLOCK(__n) spinlock_t __n = { .lock = 0UL, }
#define SPIN_LOCK_INIT(__n) { .lock = 0UL,}

#define STATIC_SPIN_LOCK_INIT { .lock = 0UL,}

CDECL
extern void spinlock_init(spinlock_t *lock);
CDECL_END

#ifdef CONFIG_SPINLOCK_DEBUG
#define raw_spin_lock(__l) spinlock_acquire(__l, __FILE__, __LINE__)
#define raw_spin_unlock(__l) spinlock_release(__l, __FILE__, __LINE__)

/* Preempt, but no resched */
#define _raw_spin_lock(__l) spin_lock_noresched(__l, __FILE__, __LINE__)
#define _raw_spin_unlock(__l) spin_unlock_noresched(__l, __FILE__, __LINE__)

#define raw_spin_lock_irq(__l, __f) spin_lock_irqsave_nosched(__l, __f, __FILE__, __LINE__)
#define raw_spin_unlock_irq(__l, __f) spin_unlock_irqrestore_nosched(__l, __f, __FILE__, __LINE__)

#define raw_spin_lock_irqsave(__l, __f) spin_lock_irqsave_noresched(__l, &__f, __FILE__, __LINE__)
#define raw_spin_unlock_irqrestore(__l, __f) spin_unlock_irqrestore_noresched(__l, &__f, __FILE__, __LINE__)

#define spin_lock_irqsave(__l, __f) spin_lock_irqsave_resched(__l, &__f, __FILE__, __LINE__)
#define spin_unlock_irqrestore(__l, __f) spin_unlock_irqrestore_resched(__l, &__f, __FILE__, __LINE__)

#define spin_lock(__l) spin_lock_resched(__l, __FILE__, __LINE__)
#define spin_unlock(__l) spin_unlock_resched(__l, __FILE__, __LINE__)

CDECL
extern void spinlock_acquire(spinlock_t *lock, const char *file, int line);
extern void spinlock_release(spinlock_t *lock, const char *file, int line);

static inline void spin_lock_resched(spinlock_t *lock, const char *file, int line)
{
	preempt_disable();
	spinlock_acquire(lock, file, line);
}

static inline void spin_unlock_resched(spinlock_t *lock, const char *file, int line)
{
	spinlock_release(lock, file, line);
	preempt_enable();
}

static inline void spin_lock_noresched(spinlock_t *lock, const char *file, int line)
{
	preempt_disable();
	spinlock_acquire(lock, file, line);
}

static inline void spin_unlock_noresched(spinlock_t *lock, const char *file, int line)
{
	spinlock_release(lock, file, line);
	preempt_enable_no_resched();
}

static inline void spin_lock_irqsave_noresched(spinlock_t *lock, unsigned long *flags,
		const char *file, int line)
{
	preempt_disable();
	irq_save_and_disable(flags);
	spinlock_acquire(lock, file, line);
}

static inline void spin_unlock_irqrestore_noresched(spinlock_t *lock, unsigned long *flags,
		const char *file, int line)
{
	spinlock_release(lock, file, line);
	irq_restore(flags);
	preempt_enable_no_resched();
}

static inline void spin_lock_irqsave_resched(spinlock_t *lock, unsigned long *flags,
		const char *file, int line)
{
	preempt_disable();
	irq_save_and_disable(flags);
	spinlock_acquire(lock, file, line);
}

static inline void spin_unlock_irqrestore_resched(spinlock_t *lock, unsigned long *flags,
		const char *file, int line)
{
	spinlock_release(lock, file, line);
	irq_restore(flags);
	preempt_enable();
}

static inline void spin_lock_irqsave_nosched(spinlock_t *lock, unsigned long *flags,
		const char *file, int line)
{
	irq_save_and_disable(flags);
	spinlock_acquire(lock, file, line);
}

static inline void spin_unlock_irqrestore_nosched(spinlock_t *lock, unsigned long *flags,
		const char *file, int line)
{
	spinlock_release(lock, file, line);
	irq_restore(flags);
}
CDECL_END

#else
#define raw_spin_lock(__l) spinlock_acquire(__l)
#define raw_spin_unlock(__l) spinlock_release(__l)

/* Preempt, but no resched */
#define _raw_spin_lock(__l) spin_lock_noresched(__l)
#define _raw_spin_unlock(__l) spin_unlock_noresched(__l)

#define raw_spin_lock_irq(__l, __f) spin_lock_irqsave_nosched(__l, __f)
#define raw_spin_unlock_irq(__l, __f) spin_unlock_irqrestore_nosched(__l, __f)

#define raw_spin_lock_irqsave(__l, __f) spin_lock_irqsave_noresched(__l, &__f)
#define raw_spin_unlock_irqrestore(__l, __f) spin_unlock_irqrestore_noresched(__l, &__f)

#define spin_lock_irqsave(__l, __f) spin_lock_irqsave_resched(__l, &__f)
#define spin_unlock_irqrestore(__l, __f) spin_unlock_irqrestore_resched(__l, &__f)

#define spin_lock(__l) spin_lock_resched(__l)
#define spin_unlock(__l) spin_unlock_resched(__l)

CDECL
extern void spinlock_acquire(spinlock_t *lock);
extern void spinlock_release(spinlock_t *lock);

static inline void spin_lock_resched(spinlock_t *lock)
{
	preempt_disable();
	spinlock_acquire(lock);
}

static inline void spin_unlock_resched(spinlock_t *lock)
{
	spinlock_release(lock);
	preempt_enable();
}

static inline void spin_lock_noresched(spinlock_t *lock)
{
	preempt_disable();
	spinlock_acquire(lock);
}

static inline void spin_unlock_noresched(spinlock_t *lock)
{
	spinlock_release(lock);
	preempt_enable_no_resched();
}

static inline void spin_lock_irqsave_noresched(spinlock_t *lock,
		unsigned long *flags)
{
	preempt_disable();
	irq_save_and_disable(flags);
	spinlock_acquire(lock);
}

static inline void spin_unlock_irqrestore_noresched(spinlock_t *lock,
		unsigned long *flags)
{
	spinlock_release(lock);
	irq_restore(flags);
	preempt_enable_no_resched();
}

static inline void spin_lock_irqsave_resched(spinlock_t *lock,
		unsigned long *flags)
{
	preempt_disable();
	irq_save_and_disable(flags);
	spinlock_acquire(lock);
}

static inline void spin_unlock_irqrestore_resched(spinlock_t *lock,
		unsigned long *flags)
{
	spinlock_release(lock);
	irq_restore(flags);
	preempt_enable();
}

static inline void spin_lock_irqsave_nosched(spinlock_t *lock,
		unsigned long *flags)
{
	irq_save_and_disable(flags);
	spinlock_acquire(lock);
}

static inline void spin_unlock_irqrestore_nosched(spinlock_t *lock,
		unsigned long *flags)
{
	spinlock_release(lock);
	irq_restore(flags);
}
#endif /* CONFIG_SPINLOCK_DEBUG */

#endif /* __SPINLOCK_H__ */

/** @} */

