/*
 *  ETA/OS - Ohh snap, panic!
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
#include <etaos/stddef.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/irq.h>
#include <etaos/panic.h>
#include <etaos/preempt.h>

void panic_P(const char *fmt, ...)
{
	va_list list;

	fprintf(stderr, "%s ", ETAOS_PANIC_STRING);

	va_start(list, fmt);
	vfprintf_P(stderr, fmt, list);
	va_end(list);

	preempt_disable();
	irq_disable();
	for(;;);
}

