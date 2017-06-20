/*
 *  ETA/OS - Mutexes
 *  Copyright (C) 2014, 2015, 2016, 2017   Michel Megens
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

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/thread.h>
#include <etaos/spinlock.h>

typedef struct mutex {
#ifdef CONFIG_MUTEX_EVENT_QUEUE
	struct thread_queue qp;
#else
	spinlock_t lock;
#endif
#ifdef CONFIG_SCHED
	struct thread *owner;
#endif
} mutex_t;

#ifdef CONFIG_MUTEX_EVENT_QUEUE

#define STATIC_MUTEX_INIT { \
		.qp = INIT_THREAD_QUEUE, \
		.owner = NULL, \
	}
#define DEFINE_MUTEX(__n) mutex_t __n = STATIC_MUTEX_INIT

CDECL

extern int mutex_wait_tmo(mutex_t *mutex, unsigned int tmo);
extern void mutex_wait(mutex_t *mutex);
extern void mutex_lock(mutex_t *mutex);
extern void mutex_unlock(mutex_t *mutex);
extern void mutex_unlock_irq(mutex_t *mutex);

static inline void mutex_init(mutex_t *mutex)
{
	thread_queue_init(&mutex->qp);
	mutex->owner = NULL;
}

CDECL_END

#else

#define DEFINE_MUTEX(__n) mutex_t __n = { SPIN_LOCK_INIT((__n).lock) }

CDECL
static inline void mutex_init(mutex_t *mutex)
{
	spinlock_init(&mutex->lock);
}

static inline void mutex_wait(mutex_t *mutex)
{
	mutex->lock.lock = 1;
	spin_lock(&mutex->lock);
}

static inline int mutex_wait_tmo(mutex_t *mutex, int tmo)
{
	mutex_wait(mutex);
	return 0;
}

static inline void mutex_unlock_irq(mutex_t *mutex)
{
	spin_unlock(&mutex->lock);
}

static inline void mutex_lock(mutex_t *mutex)
{
	spin_lock(&mutex->lock);
}

static inline void mutex_unlock(mutex_t *mutex)
{
	spin_unlock(&mutex->lock);
}
CDECL_END

#endif

#define mutex_unlock_from_irq(__m) mutex_unlock_irq(__m)

#endif

