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
	context->notify_join_queue();
	context->kill();
}

Thread::Thread(const char *name, void *arg)
{
	thread_queue_init(&this->joinq);
	this->arg = arg;
	this->base = thread_alloc(name, stl_thread_start, this, NULL);
	running = true;
}

Thread::~Thread()
{
	if(running)
		this->kill();
}

void Thread::start()
{
	thread_start(this->base);
}

void *Thread::get_arg()
{
	return this->arg;
}

void Thread::notify_join_queue()
{
	event_notify(&this->joinq);
}

void Thread::kill()
{
	running = false;
	this->notify_join_queue();
	::kill();
}

void Thread::join()
{
	if(running)
		event_wait(&this->joinq, EVENT_WAIT_INFINITE);
}

