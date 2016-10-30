/*
 *  ETA/OS - Spinlock
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

#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/preempt.h>

#include <asm/spinlock.h>
#include <asm/irq.h>

#define DEFINE_SPINLOCK(__n) spinlock_t __n = { .lock = 0, }
#define SPIN_LOCK_INIT(__n) { .lock = 0,}

#define STATIC_SPIN_LOCK_INIT { .lock = 0,}

#define spinlock_init(__l) ((__l)->lock = 0)

#define raw_spin_lock(__l) arch_spin_lock(__l)
#define raw_spin_unlock(__l) arch_spin_unlock(__l)

CDECL
/**
 * @brief Lock the given spinlock.
 * @param lock Spin lock which has to be locked.
 */
static inline void spin_lock(spinlock_t *lock)
{
#ifdef CONFIG_PREEMPT
	preempt_disable();
#endif
	raw_spin_lock(lock);
}

/**
 * @brief Unlock a given spin lock.
 * @param lock Lock which has to be unlocked.
 */
static inline void spin_unlock(spinlock_t *lock)
{
	raw_spin_unlock(lock);
#ifdef CONFIG_PREEMPT
	preempt_enable();
#endif
}

static inline void _raw_spin_lock(spinlock_t *lock)
{
	preempt_disable();
	raw_spin_lock(lock);
}

static inline void _raw_spin_unlock(spinlock_t *lock)
{
	raw_spin_unlock(lock);
	preempt_enable_no_resched();
}

static inline void _spin_lock_irqsave(spinlock_t *lock, unsigned long *flags)
{
	preempt_disable();
	irq_save_and_disable(flags);
	raw_spin_lock(lock);
}

static inline void _spin_unlock_irqrestore(spinlock_t *lock, 
					   unsigned long *flags)
{
	raw_spin_unlock(lock);
	irq_restore(flags);
	preempt_enable();
}

static inline void _raw_spin_lock_irqsave(spinlock_t *lock, unsigned long *flags)
{
	preempt_disable();
	irq_save_and_disable(flags);
	raw_spin_lock(lock);
}

static inline void _raw_spin_unlock_irqrestore(spinlock_t *lock, 
					   unsigned long *flags)
{
	raw_spin_unlock(lock);
	irq_restore(flags);
	preempt_enable_no_resched();
}

/**
 * @brief Lock a spin lock without touching the preemption settings.
 * @param lock Spin lock that has to be locked.
 * @note The local interrupts will also be disabled.
 */
static inline void raw_spin_lock_irq(spinlock_t *lock, unsigned long *flags)
{
	irq_save_and_disable(flags);
	arch_spin_lock(lock);
}

/**
 * @brief Unlock a spin lock without touching the preemption settings.
 * @param lock Spin lock that has to be unlocked.
 * @note The local interrupts will also be enabled.
 */
static inline void raw_spin_unlock_irq(spinlock_t *lock, unsigned long *flags)
{
	arch_spin_unlock(lock);
	irq_restore(flags);
}

/**
 * @brief Attempt to aquire a spin lock.
 * @param lock Lock to attempt.
 * @note If the lock cannot be aquired nothing will be done.
 * @retval 1 If the lock cannot be aquired.
 * @retval 0 If the lock has been aquired.
 */
static inline int spin_try_lock(spinlock_t *lock)
{
	if(lock->lock) {
		return 1;
	} else {
		spin_lock(lock);
		return 0;
	}
}

CDECL_END

#define spin_lock_irqsave(_l, _f) _spin_lock_irqsave(_l, &_f)
#define spin_unlock_irqrestore(_l, _f) _spin_unlock_irqrestore(_l, &_f)

#define raw_spin_lock_irqsave(__l, __f) _raw_spin_lock_irqsave(__l, &__f)
#define raw_spin_unlock_irqrestore(__l, __f) \
	_raw_spin_unlock_irqrestore(__l, &__f)


#endif /* __SPINLOCK_H__ */

