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
#include <etaos/math.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/mem.h>
#include <etaos/list.h>

static struct lottery_ticket *lottery_take_ticket(void);

/**
 * @brief Insert a new thread into a queue.
 * @param tpp Root queue pointer.
 * @param tp Thread to insert.
 */
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
		while(thread && prio(thread) <= prio(tp)) {
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

/**
 * @brief Remove a thread from a queue.
 * @param tpp Root queue pointer.
 * @param tp Thread to remove from.
 * @retval -EOK on success.
 * @return -EINVAL on error (no thread was removed).
 */
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

struct lottery_pool_head {
	struct list_head tickets;
	unsigned long num;
};

/**
 * @brief Add a new thread to a lottery run queue.
 * @param rq Run queue to add \p tp to.
 * @param tp Thread which has to be added to the given run queue.
 */
static void lottery_add_thread(struct rq *rq, struct thread *tp)
{
	struct lottery_ticket *ticket;
	double tickets;
	unsigned int idx, _tickets;

	/* check if the thread has tickets assigned yet */
	if(list_empty(&tp->se.tickets)) {
		if(tp->prio < 250) {
			tickets = -0.04;
			tickets *= tp->prio;
			tickets += 10;
		} else {
			tickets = 1;
		}

		_tickets = (unsigned int)round(tickets);

		for(idx = 0; idx < _tickets; idx++) {
			ticket = lottery_take_ticket();
			list_add(&ticket->list, &tp->se.tickets);
		}
	}

	rq->num++;
	lottery_insert_thread(&rq->rr_rq.run_queue, tp);
}

/**
 * @brief Remove a thread from the lottery run queue.
 * @param rq Run queue to remove from.
 * @param tp Thread which is to be removed.
 */
static int lottery_remove_thread(struct rq *rq, struct thread *tp)
{
	rq->num--;
	return lottery_rm_thread(&rq->rr_rq.run_queue, tp);
}

static struct lottery_pool_head lottery_tickets = {
	.tickets = STATIC_INIT_LIST_HEAD(lottery_tickets.tickets),
	.num = 0,
};

/**
 * @brief Return a lottery ticket to the allocator.
 * @param ticket Ticket to return.
 */
static void lottery_return_ticket(struct lottery_ticket *ticket)
{
	list_del(&ticket->list);
	list_add(&ticket->list, &lottery_tickets.tickets);
}

/**
 * @brief Free all memory allocated by the lottery scheduler.
 * @param tp Thread which is about to be killed.
 * @note This function does not free the actual thread. It merely free's up
 *       allocated lottery tickets (they're returned to the ticket allocator).
 */
static void lottery_kill_thread(struct thread *tp)
{
	struct list_head *carriage, *tmp;
	struct lottery_ticket *ticket;

	list_for_each_safe(carriage, tmp, &tp->se.tickets) {
		ticket = list_entry(carriage, struct lottery_ticket, list);
		lottery_return_ticket(ticket);
	}
}

#ifdef CONFIG_EVENT_MUTEX
static struct thread *lottery_thread_after(struct thread *tp)
{
	if(tp)
		return tp->se.next;
	else
		return NULL;
}
#endif

static inline bool lottery_single_thread_available(struct rq *rq)
{
	struct thread *tp;

	tp = rq->rr_rq.run_queue;
	if(tp) {
		if(tp->se.next)
			return false;
	}

	return true;
}

/**
 * @brief Get the next runnable thread from the lottery scheduler.
 * @param rq Run queue to pick a thread from.
 *
 * A new thread is selected for execution using a pseudo random number
 * generator.
 */
static struct thread *lottery_next_runnable(struct rq *rq)
{
	unsigned long num;
	struct thread *runnable;
	struct list_head *carriage;
	struct lottery_ticket *_ticket;

	if(lottery_single_thread_available(rq))
		return rq->rr_rq.run_queue;

try_again:
	num = random_m(lottery_tickets.num);

	for(runnable = rq->rr_rq.run_queue; runnable; 
			runnable = runnable->se.next) {
		list_for_each(carriage, &runnable->se.tickets) {
			_ticket = list_entry(carriage, struct lottery_ticket,
					list);
			if(_ticket->ticket == num) {
				return runnable;
			}
		}
	}

	if(rq->num > 0)
		goto try_again;

	return NULL;
}

#define LOTTERY_POOL_SIZE 20
#define LOTTERY_RESIZE_SIZE 5

/**
 * @brief Generate new lottery tickets.
 * @param num Amount of tickets to generate.
 */
static int lottery_generate_tickets(int num)
{
	struct lottery_ticket *ticket;
	int idx = 0;

	for(; idx < num; idx++) {
		ticket = kzalloc(sizeof(*ticket));
		if(!ticket)
			return -ENOMEM;

		ticket->ticket = lottery_tickets.num;
		lottery_tickets.num++;
		list_add(&ticket->list, &lottery_tickets.tickets);
	}

	return -EOK;
}

/**
 * @brief Allocate a lottery ticket.
 * @return The allocated lottery ticket.
 */
static struct lottery_ticket *lottery_take_ticket(void)
{
	struct list_head *ticket_entry;

	if(list_empty(&lottery_tickets.tickets)) {
		lottery_generate_tickets(LOTTERY_RESIZE_SIZE);
	}

	ticket_entry = lottery_tickets.tickets.next;
	list_del(ticket_entry);
	return list_entry(ticket_entry, struct lottery_ticket, list);
}

/**
 * @brief Initialise the lottery scheduler
 */
static void lottery_init(void)
{
	struct lottery_ticket *ticket;
	int idx = 0;

	for(; idx < LOTTERY_POOL_SIZE; idx++) {
		ticket = kzalloc(sizeof(*ticket));
		if(!ticket)
			return;

		ticket->ticket = idx;
		list_add(&ticket->list, &lottery_tickets.tickets);
	}

	lottery_tickets.num = LOTTERY_POOL_SIZE;
	return;
}

/**
 * @brief Lottery scheduling class.
 */
struct sched_class lottery_class = {
	.init = &lottery_init,
	.kill = &lottery_kill_thread,
	.rm_thread = &lottery_remove_thread,
	.add_thread = &lottery_add_thread,
	.next_runnable = &lottery_next_runnable,
#ifdef CONFIG_EVENT_MUTEX
	.thread_after = &lottery_thread_after,
#endif
	.post_schedule = NULL,
};

/** @} */

