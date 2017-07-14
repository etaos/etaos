/*
 *  ETA/OS - Semaphore header
 *  Copyright (C) 2014-2017   Michel Megens <dev@bietje.net>
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
 * @file semaphore.h
 */

#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

/**
 * @addtogroup sem
 */
/* @{ */
#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/thread.h>
#include <etaos/evm.h>

/**
 * @brief Semaphore descriptor.
 */
typedef struct semaphore {
	struct thread_queue qp; //!< Thread queue to wait in.
	short value; //!< Semaphore value.
} sem_t;

CDECL

extern int sem_init(sem_t *sem, short value);
extern void sem_wait(sem_t *sem);
extern void sem_signal(sem_t *sem);
extern int sem_try_wait(sem_t *sem);

CDECL_END

/* @} */
#endif

