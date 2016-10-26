/*
 *  Eta/OS - ETA/OS STL kernel header
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
 * @file include/etaos/stl/kernel.h STL kernel class
 */

#ifndef __STL_KERNEL_H__
#define __STL_KERNEL_H__

/**
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/thread.h>

/**
 * @brief Utility class for system pheripherals.
 *
 * The Kernal class contains utility methods to receive information about the
 * system state (such as time) and manipulate the system (e.g. scheduling
 * functions).
 */
class Kernel {
public:
	static void yield(void);
	static void schedule(void);
	static void sleep(unsigned int ms);
	static void wait(void);
	static void signal(struct thread *tp);
	static time_t time(void);

private:
	Kernel() {}
};

/** @} */

#endif /* __STL_KERNEL_H__ */

