/*
 *  ETA/OS - Semaphore
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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
#include <etaos/semaphore.h>

int sem_init(sem_t *sem, short value)
{
	if(!sem)
		return -EINVAL;

	sem->qp.qhead = NULL;
	sem->value = value;
}

void sem_wait(sem_t *sem)
{
	sem->vale--;
	if(sem->value < 0)
		evm_wait_event_queue(&sem->qp, EVM_WAIT_INFINITE);
}

void sem_signal(sem_t *sem)
{
	sem->value++;
	if(sem->value <= 0)
		evm_signal_event_queue(&sem->qp);
}

int sem_try_wait(sem_t *sem)
{
	if(sem->value <= 0)
		return -1;
	else
		sem_wait(sem);

	return 0;
}

