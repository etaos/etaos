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

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/thread.h>
#include <etaos/event.h>
#include <etaos/mutex.h>
#include <etaos/panic.h>

#ifdef CONFIG_MUTEX_TRACE
#include <etaos/mem.h>
#include <etaos/string.h>
#endif

#include <asm/pgm.h>

/**
 * @brief Wait for a mutex to be signaled.
 * @param mutex Mutex lock.
 * @see event_wait.
 */
void mutex_wait(mutex_t *mutex)
{
	event_wait(&mutex->qp, EVENT_WAIT_INFINITE);
}

/**
 * @brief Wait for a mutex to be signaled with a maximum wait time.
 * @param mutex Mutex lock.
 * @param tmo Time out in miliseconds.
 * @return 0 on success, non-zero a timeout has occurred.
 */
int mutex_wait_tmo(mutex_t *mutex, unsigned int tmo)
{
	return event_wait(&mutex->qp, tmo);
}

/**
 * @brief Lock a mutex.
 * @param mutex Mutex to lock.
 * @see mutex_unlock
 *
 * This function can be used recursively by the same thread without blocking.
 */
#ifdef CONFIG_MUTEX_TRACE
void __mutex_lock(mutex_t *mutex, const char *file, int line)
#else
void mutex_lock(mutex_t *mutex)
#endif
{
	struct thread *tp = current_thread();

	preempt_disable();
	if(mutex->owner != tp) {
		while(mutex->count != 0)
			raw_event_wait(&mutex->qp, EVENT_WAIT_INFINITE);
	}

#ifdef CONFIG_MUTEX_TRACE
	if(!mutex->count) {
		mutex->lock_file = strdup(file);
		mutex->lock_line = line;
	}
#endif
	mutex->count++;
	mutex->owner = tp;
}

/**
 * @brief Unlock a mutex.
 * @param mutex Mutex to unlock.
 * @see mutex_lock
 * @see mutex_unlock_irq
 */
#ifdef CONFIG_MUTEX_TRACE
void __mutex_unlock(mutex_t *mutex, const char *file, int line)
#else
void mutex_unlock(mutex_t *mutex)
#endif
{
	struct thread *tp = current_thread();

	if(mutex->owner != tp) {
#ifdef CONFIG_MUTEX_TRACE
		panic("Faulty mutex unlock! From: {%s in %s:%i} By: {%s in %s:%i}",
			mutex->owner ? mutex->owner->name : "null", mutex->lock_file,
			mutex->lock_line, tp->name, file, line);
#else
		panic("Faulty mutex unlock!");
#endif
	}

	mutex->count -= 1;
	if(mutex->count == 0) {
#ifdef CONFIG_MUTEX_TRACE
		if(mutex->lock_file) {
			kfree(mutex->lock_file);
			mutex->lock_line = -1;
		}
#endif
		mutex->owner = NULL;
		event_notify(&mutex->qp);
	}

	preempt_enable();
}

/**
 * @brief Signal a mutex from IRQ context.
 * @param mutex Mutex to signal.
 * @see mutex_wait mutex_wait_tmo
 * @warning This function should NOT be used on mutex locked by `mutex_lock`.
 */
void mutex_unlock_irq(mutex_t *mutex)
{
	event_notify_irq(&mutex->qp);
}

/** @} */
