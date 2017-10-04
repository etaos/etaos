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

#include <etaos/stl/kernel.h>
#include <etaos/stl/thread.h>

static void stl_thread_start(void *arg)
{
	Thread *context = (Thread*)arg;

	context->run(context->get_arg());
	context->kill();
}

/**
 * @brief Create a new runnable object.
 * @param name Name of th thread.
 * @param arg Argument to the thread.
 * @see thread_alloc
 */
Thread::Thread(const char *name, void *arg)
{
	thread_attr_t attrs;

	attrs.prio = SCHED_DEFAULT_PRIO;
	this->arg = arg;
	this->base = thread_alloc(name, stl_thread_start, this, &attrs);
	this->running = false;
}

/**
 * @brief Destory a thread object.
 * @see kill
 */
Thread::~Thread()
{
	if(running)
		this->kill();
}

/**
 * @brief Start the execution of the thread.
 */
void Thread::start()
{
	this->running = true;
	thread_start(this->base);
}

void *Thread::get_arg()
{
	return this->arg;
}

/**
 * @brief Stop the execution of the thread.
 *
 * Please note that this function _must_ be called within the class Thread::run
 * method.
 */
void Thread::kill()
{
	running = false;
	::kill();
}

/**
 * @brief Wait for the thread to finish execution.
 */
void Thread::join()
{
	::join(this->base);
}

/** @} */
