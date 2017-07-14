/*
 *  ETA/OS - Scheduling core
 *  Copyright (C) 2014, 2015, 2016   Michel Megens <dev@bietje.net>
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
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/mem.h>

#include <asm/timer.h>
#include <asm/sched.h>
#include <asm/io.h>

#ifndef CONFIG_SQS
static DEFINE_RQ(avr_rq, &sys_sched_class);
#endif

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

#ifdef CONFIG_SQS
struct rq *cpu_to_rq(int cpu)
{
	return sched_get_grq();
}

struct rq *sched_get_cpu_rq(void)
{
	return sched_get_grq();
}

struct rq *sched_select_rq(void)
{
	return sched_get_grq();
}

#else

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
#endif

struct clocksource *sched_get_clock(void)
{
	return avr_get_sys_clk();
}

struct avr_stack_frame {
	uint8_t r31, r30,
		r29, r28, r27, r26, r25, r24, r23, r22, r21, r20,
		r19, r18, r17, r16, r15, r14, r13, r12, r11, r10,
		r9, r8, r7, r6, r5, r4, r3, r2, r1;
#ifdef AVR_HAVE_RAMPX
	uint8_t rampx;
#endif
#ifdef AVR_HAVE_RAMPY
	uint8_t rampy;
#endif
#ifdef AVR_HAVE_RAMPZ
	uint8_t rampz;
#endif
#ifdef AVR_HAVE_RAMPD
	uint8_t rampd;
#endif
	uint8_t status;
	uint8_t r0;

#ifdef AVR_22BIT_PC
	uint8_t handle_ext;
#endif
	uint8_t handle_hi, handle_lo;
};

#include <etaos/string.h>

void sched_create_stack_frame(struct thread *tp, stack_t *stack,
				size_t stack_size, thread_handle_t handle)
{
	struct avr_stack_frame *frame;

	if(!stack || !stack_size)
		return;

	tp->stack = stack;
	tp->stack_size = stack_size;
	tp->sp = &stack[stack_size-1];
	tp->sp -= sizeof(*frame);
	frame = (struct avr_stack_frame *) tp->sp;
	memset(frame, 0, sizeof(*frame));

	frame->handle_lo = (unsigned short)handle & 0xff;
	frame->handle_hi = ((unsigned short)handle >> 8) & 0xff;

	frame->status = SREG;
	frame->r24 = ((unsigned short)tp->param) & 0xFF; /* r24 */
	frame->r25 = (((unsigned short)tp->param) >> 8) & 0xFF; /* r25 */
	tp->sp--;
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

