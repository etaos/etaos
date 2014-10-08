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
#include <etaos/thread.h>
#include <etaos/sched.h>

static struct rq avr_rq = {
	.rq_threads = NULL,
	.current = NULL,
	.switch_count = 0,
	.lock = SPIN_LOCK_INIT(avr_rq.lock),
};

struct rq *sched_get_cpu_rq(void)
{
	return &avr_rq;
}

struct rq *sched_select_rq(void)
{
	return &avr_rq;
}

void cpu_reschedule(struct rq *rq, struct thread *prev, struct thread *next)
{
}
