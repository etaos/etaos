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

/**
 * @addtogroup mutex
 * @{
 */

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/thread.h>
#include <etaos/spinlock.h>

/**
 * @brief Recursive mutex data structure.
 */
typedef struct mutex {
#ifdef CONFIG_MUTEX_EVENT_QUEUE
	struct thread_queue qp; //!< Thread queue to wait on.
	int count; //!< Recursion count.
#else
	spinlock_t lock; //!< Spinlock if `qp` cannot be used.
#endif
#ifdef CONFIG_SCHED
	struct thread *owner; //!< Owner of the mutex.
#endif
#ifdef CONFIG_MUTEX_TRACE
	char *lock_file;
	int lock_line;
#endif
} mutex_t;

#ifdef CONFIG_MUTEX_EVENT_QUEUE

/**
 * @brief Initialise a new mutex.
 * @see DEFINE_MUTEX
 *
 * Initialises a mutex variable. Usage is as following:
@code{.c}
mutex_t lock = STATIC_MUTEX_INIT;
@endcode
 */
#define STATIC_MUTEX_INIT { \
		.qp = INIT_THREAD_QUEUE, \
		.owner = NULL, \
		.count = 0, \
	}

/**
 * @brief Define a new mutex.
 * @param __n Variable name.
 * @see STATIC_MUTEX_INIT
 */
#define DEFINE_MUTEX(__n) mutex_t __n = STATIC_MUTEX_INIT

CDECL

extern int mutex_wait_tmo(mutex_t *mutex, unsigned int tmo);
extern void mutex_wait(mutex_t *mutex);
#ifdef CONFIG_MUTEX_TRACE

extern void __mutex_lock(mutex_t *mutex, const char *file, int line);
extern void __mutex_unlock(mutex_t *mutex, const char *file, int line);
#define mutex_lock(_mtx_) __mutex_lock(_mtx_, __FILE__, __LINE__)
#define mutex_unlock(_mtx_) __mutex_unlock(_mtx_, __FILE__, __LINE__)

#else /* CONFIG_MUTEX_TRACE */

extern void mutex_lock(mutex_t *mutex);
extern void mutex_unlock(mutex_t *mutex);
#endif

extern void mutex_unlock_irq(mutex_t *mutex);

/**
 * @brief Initialise a mutex.
 * @param mutex Mutex to initialise.
 * @see DEFINE_MUTEX
 * @see STATIC_MUTEX_INIT
 */
static inline void mutex_init(mutex_t *mutex)
{
	thread_queue_init(&mutex->qp);
	mutex->owner = NULL;
	mutex->count = 0;
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

/** @} */
