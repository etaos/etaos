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

#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/thread.h>
#include <etaos/mutex.h>

/**
 * @brief Condition variable datastructure.
 */
struct condition {
	mutex_t lock; //!< Mutex lock.
	struct thread_queue qp; //!< Event queue to wait on.
};

extern void condition_init(struct condition *c);
extern int condition_wait(struct condition *c);
extern int condition_wait_for(struct condition *c, unsigned int ms);
extern void condition_signal(struct condition *c);
extern int condition_bcast(struct condition *c);
extern void condition_lock(struct condition *c);
extern void condition_unlock(struct condition *c);

#endif

/** @} */
