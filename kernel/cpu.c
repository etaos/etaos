/*
 *  ETA/OS - CPU state machine
 *  Copyright (C) 2015   Michel Megens <dev@bietje.net>
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
 * @addtogroup kernel
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/compiler.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/preempt.h>
#include <etaos/cpu.h>
#include <etaos/irq.h>

static FOR_EACH_CPU(cpu_flags, unsigned long);

/**
 * @brief Notify the CPU of a specific event.
 * @param state CPU state to notify the kernel about.
 */
void cpu_notify(cpu_state_t state)
{
	int cpu = cpu_get_id();

	switch(state) {
	case IRQ_ENTER:
		set_bit(CPU_IRQ_EXEC_FLAG, &cpu_flags[cpu]);
		break;

	case IRQ_EXIT:
		clear_bit(CPU_IRQ_EXEC_FLAG, &cpu_flags[cpu]);
		break;

	case SCHED_ENTER:
		preempt_disable();
		break;
		
	case SCHED_EXIT:
		preempt_enable_no_resched();
		break;

	default:
		break;
	}
}

/**
 * @brief Get the current CPU state.
 * @param flags Parameter to store the CPU flags in.
 */
void cpu_get_state(unsigned long *flags)
{
	unsigned long _flags;
	int cpu = cpu_get_id();

	irq_save_and_disable(&_flags);
	*flags = cpu_flags[cpu];
	irq_restore(&_flags);
}

/**
 * @brief Set a specific CPU state.
 * @param flags New CPU flags to set for the current CPU.
 * @note Usually used in conjunction with cpu_get_state.
 * @see cpu_get_state.
 */
void cpu_set_state(unsigned long *flags)
{
	unsigned long _flags;
	int cpu = cpu_get_id();

	irq_save_and_disable(&_flags);
	cpu_flags[cpu] = *flags;
	irq_restore(&_flags);
}

/** @} */

