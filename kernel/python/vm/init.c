/*
 *  ETA/OS - Python VM initialization
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
 *  Copyright (C) 2017   Dean Hall
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
#include <etaos/mem.h>
#include <etaos/string.h>
#include <etaos/python.h>

extern unsigned char usrlib_img[];

int python_init(void)
{
	void *heap;
	PmReturn_t retval;

//	heap = mm_alloc_aligned(CONFIG_PYTHON_HEAP_SIZE, 4);
	heap = kzalloc(CONFIG_PYTHON_HEAP_SIZE);
	retval = pm_init(heap, CONFIG_PYTHON_HEAP_SIZE,
			 MEMSPACE_PROG, usrlib_img);
	if (retval != PM_RET_OK)
		return -EINVAL;

	return -EOK;
}

#ifdef CONFIG_SCHED
#include <etaos/thread.h>

static volatile PmReturn_t vm_status_code = PM_RET_OK;
static char pymodule[32];

THREAD(py_runner, modname)
{
	uint8_t *pymodule;

	pymodule = (uint8_t *) modname;
	vm_status_code = pm_run(pymodule);
	kill();
}

int python_status(void)
{
	return vm_status_code;
}

int python_start(const char *modname)
{
	thread_attr_t attribs;

	strcpy(&pymodule[0], modname);
	attribs.stack = kzalloc(CONFIG_PYTHON_STACK_SIZE);
	attribs.stack_size = CONFIG_PYTHON_STACK_SIZE;
	attribs.prio = CONFIG_PYTHON_PRIO;
	thread_create("python", &py_runner, &pymodule[0], &attribs);

	return -EOK;
}
#else
int python_start(const char *modname)
{
	uint8_t *pymod = (uint8_t *) modname;

	return pm_run(pymod);
}
#endif
