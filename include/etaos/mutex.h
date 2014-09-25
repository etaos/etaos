/*
 *  ETA/OS - Mutexes
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

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>

typedef struct mutex {
#ifdef CONFIG_EVENT_MUTEX
	struct thread *queue;
#else
	uint8_t lock;
#endif
#ifdef CONFIG_SCHED
	struct thread *owner;
#endif
} mutex_t;

#define mutex_lock_irqsave(__l, __f) _mutex_lock_irqsave(__l, &__f)
#define mutex_unlock_irqrestore(__l, __f) _mutex_unlock_irqrestore(__l, __f)

#ifdef CONFIG_EVENT_MUTEX
#else

#define DEFINE_MUTEX(__n) mutex_t __n = { .lock = 0, }

#include <asm/spinlock.h>

static inline void _mutex_lock_irqsave(mutex_t *lock, unsigned long *flags)
{
	irq_save_and_disable(flags);
	arch_mutex_lock(lock);
}

static inline void _mutex_unlock_irqrestore(mutex_t *lock, unsigned long flags)
{
	irq_restore(&flags);
	arch_mutex_unlock(lock);
}

#define mutex_lock(__l) arch_mutex_lock(__l)
#define mutex_unlock(__l) arch_mutex_unlock(__l)
#endif

#endif
