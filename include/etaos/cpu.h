/*
 *  ETA/OS - CPU
 *  Copyright (C) 2015   Michel Megens <dev@bietje.net>
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
 * @file include/etaos/cpu.h CPU header
 */

#ifndef __CPU_H__
#define __CPU_H__

/**
 * @addtogroup kernel
 * @{
 */

#include <etaos/kernel.h>
#include <asm/io.h>

/**
 * @brief CPU state's.
 */
typedef enum {
	SCHED_ENTER, //!< CPU entered the scheduler.
	SCHED_EXIT,  //!< CPU exits the scheduler.
	IRQ_ENTER,   //!< CPU enters an IRQ.
	IRQ_EXIT,    //!< CPU exits an IRQ.
} cpu_state_t;

/**
 * @brief Define a variable per CPU.
 * @param __name Variable name.
 * @param __type Variable type.
 */
#define FOR_EACH_CPU(__name, __type) typeof(__type) __name[CPU_CORE_NUM]

#define CPU_IRQ_EXEC_FLAG 0

CDECL
extern void cpu_get_state(unsigned long *flags);
extern void cpu_notify(cpu_state_t state);
extern void cpu_set_state(unsigned long *flags);
CDECL_END

/** @} */

#endif

