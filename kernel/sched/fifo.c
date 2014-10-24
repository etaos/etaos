/*
 *  ETA/OS - Round robin sched class
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
#include <etaos/sched.h>
#include <etaos/thread.h>

struct sched_class sys_sched_class = {
	.rm_thread = NULL,
	.add_thread = NULL,
	.next_runnable = NULL,
#ifdef CONFIG_EVENT_MUTEX
	.thread_after = NULL,
#endif
	.post_schedule = NULL,
#ifdef CONFIG_THREAD_QUEUE
	.queue_add = NULL,
	.queue_rm = NULL,
#endif
};
