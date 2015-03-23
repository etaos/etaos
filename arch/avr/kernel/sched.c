/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014, 2015   Michel Megens <dev@michelmegens.net>
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
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/mem.h>

#include <asm/time.h>
#include <asm/sched.h>
#include <asm/io.h>

static DEFINE_RQ(avr_rq, &sys_sched_class);

/*static struct rq avr_rq = {
	.sched_class = &sys_sched_class,
#if defined(CONFIG_RR) || defined(CONFIG_FIFO)
	.rr_rq = { .run_queue = NULL, },
#endif
	.wake_queue = NULL,
	.kill_queue = NULL,
	.current = NULL,
	.switch_count = 0,
	.num = 0,
	.lock = SPIN_LOCK_INIT(avr_rq.lock),

}; */

/**
 * @brief Initialise the AVR scheduling core.
 * @see avr_get_sys_clk
 *
 * Basically initialises the clocksource of the AVR run queue.
 */
void avr_init_sched(void)
{
	avr_rq.source = avr_get_sys_clk();
}

struct rq *cpu_to_rq(int cpu)
{
	return &avr_rq;
}

struct rq *sched_get_cpu_rq(void)
{
	return &avr_rq;
}

struct rq *sched_select_rq(void)
{
	return &avr_rq;
}

void sched_create_stack_frame(struct thread *tp, stack_t *stack,
				size_t stack_size, thread_handle_t handle)
{
	char i;

	if(!stack || !stack_size)
		return;

	tp->stack = stack;
	tp->stack_size = stack_size;
	tp->sp = &stack[stack_size-1];
	*(tp->sp--) = (unsigned short)handle & 0xff;
	*(tp->sp--) = ((unsigned short)handle >> 8) & 0xff;
#ifdef AVR_22BIT_PC
	*(tp->sp--) = 0;
#endif

	/* add the SREG register */
	*(tp->sp--) = 0x0; // location of r0 normally
	*(tp->sp--) = SREG;

	i = 0;
	for(; i < 31; i++)
		*(tp->sp--) = 0;

	tp->sp[8] = ((unsigned short)tp->param) & 0xFF; /* r24 */
	tp->sp[7] = (((unsigned short)tp->param) >> 8) & 0xFF; /* r25 */
}

void sched_free_stack_frame(struct thread *tp)
{
	kfree(tp->stack);
}

void avr_save_stack(stack_t *sp, struct thread *current)
{
	if(current) {
		sp += 2;
		current->sp = sp;
	}

	return;
}

void cpu_switch_context(struct rq *rq, struct thread *prev, struct thread *next)
{
	irq_enter_critical();
	avr_switch_context(next->sp, prev);
	irq_exit_critical();
}

