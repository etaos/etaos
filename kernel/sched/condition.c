/*
 *  ETA/OS - Condition variables
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
 * @addtogroup condition
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/event.h>
#include <etaos/mutex.h>
#include <etaos/condition.h>
#include <etaos/assert.h>
#include <etaos/preempt.h>

/**
 * @brief Initialise a new condition variable.
 * @param cond Condition variable to initialise.
 */
void condition_init(struct condition *cond)
{
	ASSERT(cond != NULL);

	thread_queue_init(&cond->qp);
	mutex_init(&cond->lock);
}

/**
 * @brief Wait for \p ms miliseconds on \p condi.
 * @param condi Condition variable to wait on.
 * @param ms Number of miliseconds to wait at most.
 * @return An error code.
 * @see condition_wait condition_wait_for
 * @note If \p ms is set to \p EVENT_WAIT_INFINITE, the wait will last until
 *       a signal is received.
 */
static int __condition_wait_for(struct condition *condi, unsigned int ms)
{
	int rv;

	preempt_disable();
	mutex_unlock(&condi->lock);
	rv = event_wait(&condi->qp, ms);
	mutex_lock(&condi->lock);
	preempt_enable();

	return rv;
}

/**
 * @brief Wait until another thread signals \p cond.
 * @param cond The condition to wait on.
 * @return An error code.
 */
int condition_wait(struct condition *cond)
{
	ASSERT(cond != NULL);
	return __condition_wait_for(cond, EVENT_WAIT_INFINITE);
}

/**
 * @brief Wait until another thread signals \p cond.
 * @param condi The condition to wait on.
 * @param ms Number of miliseconds to wait.
 * @return An error code.
 */
int condition_wait_for(struct condition *condi, unsigned int ms)
{
	ASSERT(condi != NULL);
	return __condition_wait_for(condi, ms);
}

/**
 * @brief Signal a condition variable.
 * @param cond Condition variable to signal.
 */
void condition_signal(struct condition *cond)
{
	ASSERT(cond != NULL);
	event_notify(&cond->qp);
}

/**
 * @brief Signal all threads waiting on a condition variable.
 * @param cond Condition variable to broadcast the signal onto.
 * @return An error code.
 */
int condition_bcast(struct condition *cond)
{
	ASSERT(cond != NULL);
	return event_notify_broadcast(&cond->qp);
}

/**
 * @brief Lock a condition variable.
 * @param condi Condition variable to lock.
 */
void condition_lock(struct condition *condi)
{
	ASSERT(condi != NULL);
	mutex_lock(&condi->lock);
}

/**
 * @brief Unlock a condition variable.
 * @param condi Condition variable to unlock.
 */
void condition_unlock(struct condition *condi)
{
	ASSERT(condi != NULL);
	mutex_unlock(&condi->lock);
}

/** @} */
