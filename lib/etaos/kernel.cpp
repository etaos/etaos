/*
 *  ETA/OS - ETA/OS STL kernel
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
#include <etaos/event.h>
#include <etaos/time.h>

#include <etaos/stl/kernel.h>

void Kernel::yield(void)
{
	::yield();
}

void Kernel::schedule(void)
{
	::schedule();
}

void Kernel::sleep(unsigned int time)
{
	::sleep(time);
}

void Kernel::wait(void)
{
	::wait();
}

void Kernel::signal(struct thread *tp)
{
	::signal(tp);
}

time_t Kernel::time(void)
{
	time_t rv;

	::time(&rv);
	return rv;
}

