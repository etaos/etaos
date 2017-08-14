/*
 *  ETA/OS - Earliest deadline first algorithm
 *  Copyright (C) 2016, 2017  Michel Megens <dev@bietje.net>
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
 * @addtogroup edf
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/thread.h>

#include <asm/pgm.h>
#include <asm/io.h>

#include "rr_shared.h"

#ifdef CONFIG_EDF_LOOKUP_TABLE
#define edf_prio_ratio(__p) (((13 * __p) / 24) + 10)

static const unsigned char __pgm __prio_array[] = {
	edf_prio_ratio(0), edf_prio_ratio(1), edf_prio_ratio(2),
	edf_prio_ratio(3), edf_prio_ratio(4), edf_prio_ratio(5),
	edf_prio_ratio(6), edf_prio_ratio(7), edf_prio_ratio(8),
	edf_prio_ratio(9), edf_prio_ratio(10), edf_prio_ratio(11),
	edf_prio_ratio(12), edf_prio_ratio(13), edf_prio_ratio(14),
	edf_prio_ratio(15), edf_prio_ratio(16), edf_prio_ratio(17),
	edf_prio_ratio(18), edf_prio_ratio(19), edf_prio_ratio(20),
	edf_prio_ratio(21), edf_prio_ratio(22), edf_prio_ratio(23),
	edf_prio_ratio(24), edf_prio_ratio(25), edf_prio_ratio(26),
	edf_prio_ratio(27), edf_prio_ratio(28), edf_prio_ratio(29),
	edf_prio_ratio(30), edf_prio_ratio(31), edf_prio_ratio(32),
	edf_prio_ratio(33), edf_prio_ratio(34), edf_prio_ratio(35),
	edf_prio_ratio(36), edf_prio_ratio(37), edf_prio_ratio(38),
	edf_prio_ratio(39), edf_prio_ratio(40), edf_prio_ratio(41),
	edf_prio_ratio(42), edf_prio_ratio(43), edf_prio_ratio(44),
	edf_prio_ratio(45), edf_prio_ratio(46), edf_prio_ratio(47),
	edf_prio_ratio(48), edf_prio_ratio(49), edf_prio_ratio(50),
	edf_prio_ratio(51), edf_prio_ratio(52), edf_prio_ratio(53),
	edf_prio_ratio(54), edf_prio_ratio(55), edf_prio_ratio(56),
	edf_prio_ratio(57), edf_prio_ratio(58), edf_prio_ratio(59),
	edf_prio_ratio(60), edf_prio_ratio(61), edf_prio_ratio(62),
	edf_prio_ratio(63), edf_prio_ratio(64), edf_prio_ratio(65),
	edf_prio_ratio(66), edf_prio_ratio(67), edf_prio_ratio(68),
	edf_prio_ratio(69), edf_prio_ratio(70), edf_prio_ratio(71),
	edf_prio_ratio(72), edf_prio_ratio(73), edf_prio_ratio(74),
	edf_prio_ratio(75), edf_prio_ratio(76), edf_prio_ratio(77),
	edf_prio_ratio(78), edf_prio_ratio(79), edf_prio_ratio(80),
	edf_prio_ratio(81), edf_prio_ratio(82), edf_prio_ratio(83),
	edf_prio_ratio(84), edf_prio_ratio(85), edf_prio_ratio(86),
	edf_prio_ratio(87), edf_prio_ratio(88), edf_prio_ratio(89),
	edf_prio_ratio(90), edf_prio_ratio(91), edf_prio_ratio(92),
	edf_prio_ratio(93), edf_prio_ratio(94), edf_prio_ratio(95),
	edf_prio_ratio(96), edf_prio_ratio(97), edf_prio_ratio(98),
	edf_prio_ratio(99), edf_prio_ratio(100), edf_prio_ratio(101),
	edf_prio_ratio(102), edf_prio_ratio(103), edf_prio_ratio(104),
	edf_prio_ratio(105), edf_prio_ratio(106), edf_prio_ratio(107),
	edf_prio_ratio(108), edf_prio_ratio(109), edf_prio_ratio(110),
	edf_prio_ratio(111), edf_prio_ratio(112), edf_prio_ratio(113),
	edf_prio_ratio(114), edf_prio_ratio(115), edf_prio_ratio(116),
	edf_prio_ratio(117), edf_prio_ratio(118), edf_prio_ratio(119),
	edf_prio_ratio(120), edf_prio_ratio(121), edf_prio_ratio(122),
	edf_prio_ratio(123), edf_prio_ratio(124), edf_prio_ratio(125),
	edf_prio_ratio(126), edf_prio_ratio(127), edf_prio_ratio(128),
	edf_prio_ratio(129), edf_prio_ratio(130), edf_prio_ratio(131),
	edf_prio_ratio(132), edf_prio_ratio(133), edf_prio_ratio(134),
	edf_prio_ratio(135), edf_prio_ratio(136), edf_prio_ratio(137),
	edf_prio_ratio(138), edf_prio_ratio(139), edf_prio_ratio(140),
	edf_prio_ratio(141), edf_prio_ratio(142), edf_prio_ratio(143),
	edf_prio_ratio(144), edf_prio_ratio(145), edf_prio_ratio(146),
	edf_prio_ratio(147), edf_prio_ratio(148), edf_prio_ratio(149),
	edf_prio_ratio(150), edf_prio_ratio(151), edf_prio_ratio(152),
	edf_prio_ratio(153), edf_prio_ratio(154), edf_prio_ratio(155),
	edf_prio_ratio(156), edf_prio_ratio(157), edf_prio_ratio(158),
	edf_prio_ratio(159), edf_prio_ratio(160), edf_prio_ratio(161),
	edf_prio_ratio(162), edf_prio_ratio(163), edf_prio_ratio(164),
	edf_prio_ratio(165), edf_prio_ratio(166), edf_prio_ratio(167),
	edf_prio_ratio(168), edf_prio_ratio(169), edf_prio_ratio(170),
	edf_prio_ratio(171), edf_prio_ratio(172), edf_prio_ratio(173),
	edf_prio_ratio(174), edf_prio_ratio(175), edf_prio_ratio(176),
	edf_prio_ratio(177), edf_prio_ratio(178), edf_prio_ratio(179),
	edf_prio_ratio(180), edf_prio_ratio(181), edf_prio_ratio(182),
	edf_prio_ratio(183), edf_prio_ratio(184), edf_prio_ratio(185),
	edf_prio_ratio(186), edf_prio_ratio(187), edf_prio_ratio(188),
	edf_prio_ratio(189), edf_prio_ratio(190), edf_prio_ratio(191),
	edf_prio_ratio(192), edf_prio_ratio(193), edf_prio_ratio(194),
	edf_prio_ratio(195), edf_prio_ratio(196), edf_prio_ratio(197),
	edf_prio_ratio(198), edf_prio_ratio(199), edf_prio_ratio(200),
	edf_prio_ratio(201), edf_prio_ratio(202), edf_prio_ratio(203),
	edf_prio_ratio(204), edf_prio_ratio(205), edf_prio_ratio(206),
	edf_prio_ratio(207), edf_prio_ratio(208), edf_prio_ratio(209),
	edf_prio_ratio(210), edf_prio_ratio(211), edf_prio_ratio(212),
	edf_prio_ratio(213), edf_prio_ratio(214), edf_prio_ratio(215),
	edf_prio_ratio(216), edf_prio_ratio(217), edf_prio_ratio(218),
	edf_prio_ratio(219), edf_prio_ratio(220), edf_prio_ratio(221),
	edf_prio_ratio(222), edf_prio_ratio(223), edf_prio_ratio(224),
	edf_prio_ratio(225), edf_prio_ratio(226), edf_prio_ratio(227),
	edf_prio_ratio(228), edf_prio_ratio(229), edf_prio_ratio(230),
	edf_prio_ratio(231), edf_prio_ratio(232), edf_prio_ratio(233),
	edf_prio_ratio(234), edf_prio_ratio(235), edf_prio_ratio(236),
	edf_prio_ratio(237), edf_prio_ratio(238), edf_prio_ratio(239),
	edf_prio_ratio(240), edf_prio_ratio(241), edf_prio_ratio(242),
	edf_prio_ratio(243), edf_prio_ratio(244), edf_prio_ratio(245),
	edf_prio_ratio(246), edf_prio_ratio(247), edf_prio_ratio(248),
	edf_prio_ratio(249), edf_prio_ratio(250), edf_prio_ratio(251),
	edf_prio_ratio(252), edf_prio_ratio(253), edf_prio_ratio(254),
	edf_prio_ratio(255)
};

/**
 * @brief Calculate the EDF priority ratio.
 * @param prio Nice value to calculate the ratio for.
 * @return The priority ratio for \p pio.
 *
 * The priority ratio is calculated using the following formula:\n\n
 * \f$f(p) = \frac{13}{24}p + 10\f$\n\n
 * Where:
 * * \f$f(p)\f$ is the priority ratio
 * * \f$p\f$ is the nice value
 */
static inline int edf_calc_ratio(unsigned char prio)
{
	return pgm_read_byte(__prio_array + prio);
}
#else
static inline int edf_calc_ratio(unsigned char prio)
{
	return ((13 * prio) / 24) + 10;
}
#endif

/**
 * @brief Check whether or not a deadline occurrs before another.
 * @param node Deadline of a node already in the run queue.
 * @param this Deadline to check against \p node.
 * @return True if this <= node, false otherwise.
 */
static bool edf_sort_before(time_t node, time_t this)
{
	return this <= node;
}

/**
 * @brief Get the deadline from a round robin entity.
 * @param entity Round robin entity to get the deadline from.
 * @return The deadline stored in \p entity.
 */
static time_t deadline(struct rr_entity *entity)
{
	return entity->deadline;
}

/**
 * @brief Insert a thread into a raw queue using EDF.
 * @param tpp Raw queue pointer to insert into.
 * @param tp Thread pointer to insert.
 * @return An error code (-EOK on success).
 */
static int raw_edf_insert(struct thread *volatile*tpp, struct thread *tp)
{
	struct thread *thread;
	struct rr_entity *se;
	struct clocksource *clock;

#ifdef CONFIG_EVENT_MUTEX
	tp->ec = 0;
#endif

	clock = sched_get_clock();
	se = &tp->se;
	tp->queue = tpp;
	thread = *tpp;
	se->deadline = clocksource_get_tick(clock) + edf_calc_ratio(prio(tp));

	if(test_bit(THREAD_IDLE_FLAG, &tp->flags))
		se->deadline += 15778463000000LL;

	if(thread == SIGNALED) {
		thread = NULL;
#ifdef CONFIG_EVENT_MUTEX
		tp->ec++;
#endif
	} else if(thread) {
		while(thread &&
			!edf_sort_before(deadline(&thread->se), deadline(se))) {
			tpp = &thread->se.next;
			thread = thread->se.next;
		}
	}
	
	se->next = thread;
	*tpp = tp;

#ifdef CONFIG_EVENT_MUTEX
	if(se->next && se->next->ec) {
		tp->ec += se->next->ec;
		se->next->ec = 0;
	}
#endif

	return -EOK;
}

#ifdef CONFIG_THREAD_QUEUE
/**
 * @brief Add a new thread to a queue.
 * @param qp Queue to add the thread to.
 * @param tp Thread to add.
 */
static void edf_thread_queue_add(struct thread_queue *qp, struct thread *tp)
{
	raw_edf_insert(&qp->qhead, tp);
}

/**
 * @brief Remove a thread from a queue.
 * @param qp Queue to remove from.
 * @param tp Thread to remove.
 */
static void edf_thread_queue_remove(struct thread_queue *qp, struct thread *tp)
{
	rr_shared_queue_remove(&qp->qhead, tp);
}
#endif

/**
 * @brief Add a new thread to the run queue.
 * @param rq Run queue to add to.
 * @param tp Thread to add.
 */
static void edf_add_thread(struct rq *rq, struct thread *tp)
{
	struct rr_rq *rr;

	rr = &rq->rr_rq;
	if(raw_edf_insert(&rr->run_queue, tp) == -EOK)
		rq->num++;
}

/**
 * @brief Remove a thread from a rq.
 * @param rq Run queue to remove from.
 * @param tp Thread to remove.
 * @retval -EOK on success.
 * @return -EINVAL on error (no thread was removed).
 */
static int edf_rm_thread(struct rq *rq, struct thread *tp)
{
	int rc;

	if((rc = rr_shared_queue_remove(&rq->rr_rq.run_queue, tp)) == -EOK)
		rq->num--;
	
	return rc;
}

/**
 * @brief Get the next runnable thread on the run queue.
 * @param rq RQ to get the next runnable thread from.
 * @return The next runnable thread.
 * @retval NULL if no runnable thread was found.
 */
static struct thread *edf_next_runnable(struct rq *rq)
{
	struct thread *runnable;

	for(runnable = rq->rr_rq.run_queue; runnable; 
			runnable = runnable->se.next) {
		if(!test_bit(THREAD_RUNNING_FLAG, &runnable->flags))
			continue;
		else
			break;
	}
	
	return runnable;
}

#ifdef CONFIG_PREEMPT
/**
 * @brief Check if the current thread should be preempted.
 * @param rq The run queue we're on.
 * @param cur The current thread.
 * @param nxt The thread that is to be ran next.
 * @return True if \p cur should be preempted, false otherwise.
 *
 * This function only checks if the thread should be preempted according
 * to the EDF principles. It has no regard to the time slice (__schedule
 * handles time slice preemption).
 */
static bool edf_preempt_chk(struct rq *rq,
		struct thread *cur, struct thread *nxt)
{
	time_t d1, d2;

	d1 = deadline(&cur->se);
	d2 = deadline(&nxt->se);

	/*
	 * Do not preempt if currents deadline is smaller than
	 * the next deadline
	 */
	if(d1 < d2)
		return false;
	else if(d2 < d1)
		return true;
	else
		return prio(nxt) < prio(cur);
}
#endif

#ifdef CONFIG_EVENT_MUTEX
/**
 * @brief Get the next runnable thread after \p tp.
 * @param tp Thread to get the 'next' of.
 * @return The next of \p tp.
 * @retval NULL if \p tp has no next thread.
 */
static struct thread *edf_thread_after(struct thread *tp)
{
	if(tp)
		return tp->se.next;
	else
		return NULL;
}
#endif

#ifdef CONFIG_SCHED_DBG
/**
 * Print information about the EDF run queue.
 *
 * @param rq Run queue to print out.
 */
static void edf_print_rq(struct rq *rq)
{
	struct rr_rq *rr;
	struct thread *tp;

	rr = &rq->rr_rq;
	tp = rr->run_queue;

	while(tp) {
#ifdef CONFIG_PREEMPT
		printf("Name: %s - Preempt count: %i - Flags: %lu\n",
				tp->name, tp->preempt_cnt, tp->flags);
#else
		printf("Name: %s - Flags: %lu\n", tp->name, tp->flags);
#endif
		tp = tp->se.next;
	}
}
#endif

/**
 * @brief EDF scheduling class.
 */
struct sched_class edf_class = {
	.rm_thread = &edf_rm_thread,
	.add_thread = &edf_add_thread,
	.next_runnable = &edf_next_runnable,
#ifdef CONFIG_PREEMPT
	.preempt_chk = &edf_preempt_chk,
#endif
#ifdef CONFIG_EVENT_MUTEX
	.thread_after = &edf_thread_after,
#endif
	.post_schedule = NULL,
#ifdef CONFIG_THREAD_QUEUE
	.queue_add = &edf_thread_queue_add,
	.queue_rm = &edf_thread_queue_remove,
#endif
#ifdef CONFIG_SCHED_DBG
	.print_rq = &edf_print_rq,
#endif
};

/**
 * @}
 */
