/*
 *  ETA/OS - C++ support
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
#include <etaos/mem.h>

void *operator new(size_t num)
{
	return kmalloc(num);
}

void operator delete(void *ptr)
{
	if(!ptr)
		kfree(ptr);
}

int __cxa_guard_acquire(__guard *g) 
{
	return !*(char *)(g);
}

void __cxa_guard_release (__guard *g) 
{
	*(char *)g = 1;
}

void __cxa_pure_virtual(void)
{
	/*
	 * TODO: Do some big time complaining here
	 */
}

void __cxa_guard_abort (__guard *)
{
}

