/*
 *  ETA/OS - Lottery scheduling algorithm
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

/**
 * @addtogroup lottery
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdlib.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/mem.h>

static void lottery_insert_thread(struct thread *volatile*tpp, 
		struct thread *tp)
{
	struct thread *thread;
#ifdef CONFIG_EVENT_MUTEX
	tp->ec = 0;
#endif
	tp->queue = tpp;
	
	thread = *tpp;

	if(thread == SIGNALED) {
		thread = NULL;
#ifdef CONFIG_EVENT_MUTEX
		tp->ec++;
#endif
	} else if(thread) {
		while(thread) {
			tpp = &thread->se.next;
			thread = thread->se.next;
		}
	}

	tp->se.next = thread;
	*tpp = tp;
#ifdef CONFIG_EVENT_MUTEX
	if(tp->se.next && tp->se.next->ec) {
		tp->ec += tp->se.next->ec;
		tp->se.next->ec = 0;
	}
#endif
}

static int lottery_rm_thread(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;
	int efifo = -EINVAL;

	thread = *tpp;

	if(thread == SIGNALED)
		return efifo;

	while(thread) {
		if(thread == tp) {
			efifo = 0;
			*tpp = tp->se.next;
#ifdef CONFIG_EVENT_MUTEX
			if(tp->ec) {
				if(tp->se.next)
					tp->se.next->ec = tp->ec;
				tp->ec = 0;
			}
#endif

			tp->se.next = NULL;
			tp->queue = NULL;
			break;
		}

		tpp = &thread->se.next;
		thread = thread->se.next;
	}

	return efifo;
}

static void lottery_add_thread(struct rq *rq, struct thread *tp)
{
	rq->num++;
	lottery_insert_thread(&rq->rr_rq.run_queue, tp);
}

static int lottery_remove_thread(struct rq *rq, struct thread *tp)
{
	rq->num--;
	return lottery_rm_thread(&rq->rr_rq.run_queue, tp);
}

static struct thread *lottery_thread_after(struct thread *tp)
{
	if(tp)
		return tp->se.next;
	else
		return NULL;
}

static struct thread *lottery_next_runnable(struct rq *rq)
{
	return NULL;
}

struct lottery_ticket_pool {
	unsigned long num;
	long *tickets;
};

static struct lottery_ticket_pool lottery_pool = {
	.num = 0,
	.tickets = NULL,
};

#define LOTTERY_POOL_SIZE 20
#define LOTTERY_RESIZE_SIZE 5

static void lottery_realloc_pool(int resize)
{
}

static unsigned short lottery_take_ticket(void)
{
	unsigned short val = -1;
	unsigned long idx;

	for(idx = 0; idx < lottery_pool.num; idx++) {
		if(lottery_pool.tickets[idx] < 0)
			continue;
		val = lottery_pool.tickets[idx] & 0xFFFF;
		lottery_pool.tickets[idx] = -1;
	}

	if(val == -1) {
		lottery_realloc_pool(LOTTERY_RESIZE_SIZE);
		val = lottery_pool.tickets[idx] & 0xFFFF;
		lottery_pool.tickets[idx] = -1;
	}

	return val;
}

static void lottery_init(void)
{
	char i;

	lottery_pool.num = LOTTERY_POOL_SIZE;
	lottery_pool.tickets = kzalloc(sizeof(long)*LOTTERY_POOL_SIZE);

	for(i = 0; i < LOTTERY_POOL_SIZE; i++) {
		lottery_pool.tickets[i] = i;
	}
}

/**
 * @brief FIFO scheduling class.
 */
struct sched_class lottery_class = {
	.init = &lottery_init,
	.rm_thread = &lottery_remove_thread,
	.add_thread = &lottery_add_thread,
	.next_runnable = &lottery_next_runnable,
#ifdef CONFIG_EVENT_MUTEX
	.thread_after = &lottery_thread_after,
#endif
	.post_schedule = NULL,
};

/** @} */
