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

/**
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/event.h>
#include <etaos/time.h>

#include <etaos/stl/kernel.h>

/**
 * @brief Yield the current thread.
 *
 * Check if there are other threads of higher priority than the current one,
 * and if so reschedule the current thread.
 */
void Kernel::yield(void)
{
	::yield();
	Kernel::schedule();
}

/**
 * @brief Reschedule the current thread.
 *
 * The current thread will only be rescheduled if the scheduler deems it
 * neccessary.
 */
void Kernel::schedule(void)
{
	::schedule();
}

/**
 * @brief Sleep the current thread.
 * @param time Time to sleep in milliseconds.
 */
void Kernel::sleep(unsigned int time)
{
	::sleep(time);
}

/**
 * @brief Put the current thread in a waiting state.
 * @see Kernel::signal
 *
 * The current thread will keep waiting until it is signalled using #signal.
 */
void Kernel::wait(void)
{
	::wait();
}

/**
 * @brief Signal a waiting thread.
 * @param tp Thread to signal.
 * @see Kernel::wait
 */
void Kernel::signal(struct thread *tp)
{
	::signal(tp);
}

/**
 * @brief Get the current time stamp.
 * @return The current time stamp.
 */
time_t Kernel::time(void)
{
	time_t rv;

	::time(&rv);
	return rv;
}

/** @} */

