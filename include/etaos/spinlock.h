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

#define raw_spin_lock(__l) arch_spin_lock(__l)
#define raw_spin_unlock(__l) arch_spin_unlock(__l)

static inline void spin_lock(spinlock_t *lock)
{
#ifdef CONFIG_PREEMPT
	preempt_disable();
#endif
	raw_spin_lock(lock);
}

static inline void spin_unlock(spinlock_t *lock)
{
	raw_spin_unlock(lock);
#ifdef CONFIG_PREEMPT
	preempt_enable();
#endif
}

#define DEFINE_SPINLOCK(__n) spinlock_t __n = { .lock = 0, }
#define SPIN_LOCK_INIT(__n) { .lock = 0, }

#define STATIC_SPIN_LOCK_INIT { .lock = 0, }

#define spinlock_init(__l) ((__l)->lock = 0)

static inline void _spin_lock_irqsave(spinlock_t *lock, unsigned long *flags)
{
	irq_save_and_disable(flags);
	spin_lock(lock);
}

static inline void _spin_unlock_irqrestore(spinlock_t *lock, 
					   unsigned long *flags)
{
	irq_restore(flags);
	spin_unlock(lock);
}

static inline void _raw_spin_lock_irqsave(spinlock_t *lock, unsigned long *flags)
{
	raw_spin_lock(lock);
	irq_save_and_disable(flags);
}

static inline void _raw_spin_unlock_irqrestore(spinlock_t *lock, 
					   unsigned long *flags)
{
	irq_restore(flags);
	raw_spin_unlock(lock);
}

static inline void raw_spin_lock_irq(spinlock_t *lock)
{
	local_irq_disable();
	arch_spin_lock(lock);
}

static inline void raw_spin_unlock_irq(spinlock_t *lock)
{
	local_irq_enable();
	arch_spin_unlock(lock);
}

#define spin_lock_irqsave(__l, __f) _spin_lock_irqsave(__l, &__f)
#define spin_unlock_irqrestore(__l, __f) _spin_unlock_irqrestore(__l, &__f)
#define raw_spin_lock_irqsave(__l, __f) _raw_spin_lock_irqsave(__l, &__f)
#define raw_spin_unlock_irqrestore(__l, __f) \
	_raw_spin_unlock_irqrestore(__l, &__f)

#endif /* __SPINLOCK_H__ */

