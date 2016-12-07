/*
 *  ETA/OS - ETA/OS STL thread
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/event.h>

#ifndef __STL_THREAD_H__
#define __STL_THREAD_H__

/**
 * @brief Interface class for runnable objects.
 *
 * The Thread class is an interface class which can be implemented by any
 * class whose instances are intended to be executed as a thread. The only
 * method that the class _must_ implement is the class Thread::run method.
 */
class Thread {
public:
	explicit Thread(const char *name, void *arg);
	virtual ~Thread();

	virtual void run(void *arg) = 0;
	void start();
	void kill();
	void join();
	void *get_arg();

private:
	bool running;
	void *arg;
	struct thread *base;
};

#endif

/** @} */

