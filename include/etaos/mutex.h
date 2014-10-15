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
#include <etaos/thread.h>

typedef struct mutex {
#ifdef CONFIG_MUTEX_EVENT_QUEUE
	struct thread_queue qp;
#else
	uint8_t lock;
#endif
#ifdef CONFIG_SCHED
	struct thread *owner;
#endif
} mutex_t;

#ifdef CONFIG_MUTEX_EVENT_QUEUE
#include <etaos/thread.h>
#include <etaos/evm.h>
#define STATIC_MUTEX_INIT { \
		.qp = INIT_THREAD_QUEUE, \
		.owner = NULL, \
	}
#define DEFINE_MUTEX(__n) mutex_t __n = STATIC_MUTEX_INIT

static inline void mutex_init(mutex_t *mutex)
{
	thread_queue_init(&mutex->qp);
	mutex->owner = NULL;
}

static inline void mutex_lock(mutex_t *mutex)
{
	evm_wait_event_queue(&mutex->qp, EVM_WAIT_INFINITE);
	mutex->owner = current_thread();
}

static inline void mutex_unlock(mutex_t *mutex)
{
	mutex->owner = NULL;
	evm_signal_event_queue(&mutex->qp);
}

static inline void mutex_unlock_from_irq(mutex_t *mutex)
{
	mutex->owner = NULL;
	evm_signal_from_irq(&mutex->qp);
}

#else

#include <asm/mutex.h>

#define DEFINE_MUTEX(__n) mutex_t __n = { .lock = 0, }

static inline void mutex_init(mutex_t *mutex)
{
	mutex->lock = 0;
}

#define mutex_lock(__l) arch_mutex_lock(__l)
#define mutex_unlock(__l) arch_mutex_unlock(__l)
#endif

#endif

