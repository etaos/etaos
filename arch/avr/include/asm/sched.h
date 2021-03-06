/*
 *  Eta/OS - AVR scheduler
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#ifndef __AVR_SCHED_H_
#define __AVR_SCHED_H_

#include <etaos/kernel.h>
#include <etaos/types.h>

#include <asm/io.h>

CDECL
extern void avr_save_stack(stack_t *stack, struct thread *current);
extern void avr_switch_context(stack_t *s, struct thread *tp);
extern struct rq *sched_get_cpu_rq(void);

#define RQ_FOREACH(__rq__) \
	for(__rq__ = sched_get_cpu_rq(); __rq__; __rq__ = NULL)

CDECL_END

#endif

