/*
 *  ETA/OS - Scheduling core
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/irq.h>
#include <etaos/spinlock.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>

struct thread *thread_create(char *name, thread_handle_t handle, void *arg,
			size_t stack_size, void *stack, unsigned char prio)
{
	struct thread *tp;

	tp = kmalloc(sizeof(*tp));
	if(!tp)
		return NULL;

	if(thread_initialise(tp, name, handle, arg, stack_size, stack, prio))
		return NULL;
	return tp;
}

int thread_initialise(struct thread *tp, char *name, thread_handle_t handle, 
		void *arg, size_t stack_size, void *stack, unsigned char prio)
{
	struct rq *rq;

	if(!tp)
		return -EINVAL;

	tp->name = name;
	tp->param = arg;
	tp->preemt_cnt = 0;
	tp->prio = prio;
	tp->flags = 0;

	tp->rq_next = NULL;
	tp->queue = NULL;

	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	rq = sched_select_rq();
	
	return -EOK;
}

