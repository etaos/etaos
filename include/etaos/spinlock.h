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

#include <asm/spinlock.h>

#define spin_lock(__l) arch_spin_lock(__l)
#define spin_unlock(__l) arch_spin_unlock(__l)

#define DEFINE_SPINLOCK(__n) spinlock_t __n = { .lock = 0, }

static inline void _spin_lock_irqsave(spinlock_t *lock, unsigned long *flags)
{
	irq_save_and_disable(flags);
	arch_spin_lock(lock);
}

static inline void _spin_unlock_irqrestore(spinlock_t *lock, 
					   unsigned long *flags)
{
	arch_spin_unlock(lock);
	irq_restore(flags);
}

#define spin_lock_irqsave(__l, __f) _spin_lock_irqsave(__l, &__f)
#define spin_unlock_irqrestore(__l, __f) _spin_unlock_irqrestore(__l, &__f)

#endif /* __SPINLOCK_H__ */

