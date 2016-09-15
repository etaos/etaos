/*
 *  ETA/OS - Round robin shared functions
 *  Copyright (C) 2014, 2015   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/thread.h>

#include "rr_shared.h"

/**
 * @brief Insert a new thread into a queue.
 * @param tpp Root queue pointer.
 * @param tp Thread to insert.
 */
void rr_shared_queue_insert(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;
#ifdef CONFIG_EVENT_MUTEX
	tp->ec = 0;
#endif
	tp->queue = tpp;
	
	thread = *tpp;

	if(thread == SIGNALED) {
		thread = NULL;
#ifdef CONFIG_EVENT_MUTEX
		tp->ec++;
#endif
	} else if(thread) {
		while(thread && prio(thread) <= prio(tp)) {
			tpp = &thread->se.next;
			thread = thread->se.next;
		}
	}
	tp->se.next = thread;
	*tpp = tp;
#ifdef CONFIG_EVENT_MUTEX
	if(tp->se.next && tp->se.next->ec) {
		tp->ec += tp->se.next->ec;
		tp->se.next->ec = 0;
	}
#endif
}

/**
 * @brief Remove a thread from a queue.
 * @param tpp Root queue pointer.
 * @param tp Thread to remove from.
 * @retval -EOK on success.
 * @return -EINVAL on error (no thread was removed).
 */
int rr_shared_queue_remove(struct thread *volatile*tpp, 
		struct thread *tp)
{
	struct thread *thread;
	int err = -EINVAL;

	thread = *tpp;

	if(thread == SIGNALED)
		return err;

	while(thread) {
		if(thread == tp) {
			err = 0;
			*tpp = tp->se.next;
#ifdef CONFIG_EVENT_MUTEX
			if(tp->ec) {
				if(tp->se.next)
					tp->se.next->ec = tp->ec;
				tp->ec = 0;
			}
#endif

			tp->se.next = NULL;
			tp->queue = NULL;
			break;
		}

		tpp = &thread->se.next;
		thread = thread->se.next;
	}

	return err;
}

