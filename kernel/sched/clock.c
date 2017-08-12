/*
 *  ETA/OS - Scheduling clock
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
 * @addtogroup sched
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/bitops.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/preempt.h>
#include <etaos/cpu.h>

#ifdef CONFIG_PREEMPT
/**
 * @brief Preempt the CPU based on the context.
 * @see preempt_schedule_irq
 * @see preempt_schedule
 *
 * Check in what context the CPU is running (IRQ or non-IRQ) and preempt using
 * preempt_schedule_irq() or preempt_schedule() respectively.
 */
static void sched_clock_preempt(void)
{
	unsigned long flags;

	cpu_get_state(&flags);
	if(likely(test_bit(CPU_IRQ_EXEC_FLAG, &flags)))
		preempt_schedule_irq();
	else
		preempt_schedule();
}
#endif

#if defined(CONFIG_SCHED_FAIR) || defined(CONFIG_PREEMPT)
/**
 * @brief Update the sched clock.
 * @param ms Number of miliseconds since last call.
 * @note This function should not be called more than once per milisecond.
 */
void sched_clock_tick(int ms)
{
	struct rq *rq;
	struct thread *tp;

	rq = sched_get_cpu_rq();
	tp = rq->current;

#ifdef CONFIG_SCHED_FAIR
	tp->cputime += ms;
#endif

#ifdef CONFIG_PREEMPT
	tp->slice -= ms;
	if(!tp->slice) {
		tp->slice = CONFIG_TIME_SLICE;
		set_bit(PREEMPT_NEED_RESCHED_FLAG, &tp->flags);
	}

	sched_clock_preempt();
#endif
}
#endif /* CONFIG_SCHED_FAIR || CONFIG_PREEMPT */

/** @} */
