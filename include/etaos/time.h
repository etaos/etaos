/*
 *  ETA/OS - Timer header
 *  Copyright (C) 2014   Michel Megens
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
 * @file etaos/time.h Time header.
 * @addtogroup tm Timer module
 * @{
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/atomic.h>
#include <etaos/spinlock.h>

/**
 * @struct clocksource
 * @brief The clocksource describes the source of a hardware time.
 */
struct clocksource {
	const char *name; //!< Timer name.

	/**
	 * @brief Enable the timer.
	 * @param cs Clocksource to enable.
	 */
	int (*enable)(struct clocksource* cs);
	/**
	 * @brief disable the timer.
	 * @param cs Clocksource to disable.
	 */
	void (*disable)(struct clocksource* cs);
	unsigned long freq; //!< Frequency of the source (in Hz).
	atomic64_t tc; //!< Tick counter.
	/**
	 * @brief Tick count when last updated.
	 *
	 * Every time a clocksource is updated, this field is used to
	 * calculate how many ticks have passed since the previous update. As
	 * soon as the update is done, tc_resume is set to tc.
	 */
	int64_t tc_resume;
	spinlock_t lock; //!< Clocksource lock.

	struct list_head list; //!< List of clocksources.
	struct list_head timers; //!< List head for assigned timers.
};

/**
 * @struct timer
 * @brief Describes a single (virtual) timer.
 */
struct timer {
	struct list_head list; //!< Timer list.

	struct clocksource *source; //!< Source of the timer.
	/**
	 * @brief Timer trigger.
	 * @param timer 'This' timer.
	 * @param arg Argument. Set to priv_data.
	 *
	 * This handle is called everytime tleft hits zero.
	 */
	void (*handle)(struct timer *timer, void *arg);
	void *priv_data; //!< Private timer data.

	unsigned long tleft; //!< Ticks left.
	unsigned long ticks; //!< Total ticks,used for non ONE_SHOT timers.
};

/**
 * @def TIMER_ONESHOT_FLAG
 * @brief Sets the timer to one shot mode.
 *
 * One shot mode means that the timer will only trigger once. After it has
 * triggered it will kill itself.
 */
#define TIMER_ONESHOT_FLAG 0

/**
 * @def TIMER_ONESHOT_MASK
 * @brief Mask for a one shot timer.
 */
#define TIMER_ONESHOT_MASK (1<<TIMER_ONESHOT_FLAG)

/**
 * @brief Update the timer source.
 * @param source Clock source which needs to be updated.
 * @return The time difference, in ticks, since the last update.
 * @warning Applications should NOT use this function.
 */
static inline int64_t tm_update_source(struct clocksource *source)
{
	int64_t diff;

	diff = atomic64_get(&source->tc);
	diff -= source->tc_resume;
	return diff;
}

extern struct timer *tm_create_timer(struct clocksource *cs, unsigned long ms,
		void (*handle)(struct timer*,void*), void *arg,
		unsigned long flags);
extern int tm_clock_source_initialise(const char *name, struct clocksource *cs,
		unsigned long freq, int (*enable)(struct clocksource *cs),
		void (*disable)(struct clocksource *cs));
extern int tm_stop_timer(struct timer *timer);
extern void tm_process_clock(struct clocksource *cs, int64_t diff);
extern struct clocksource *tm_get_source_by_name(const char *name);

#endif /* __TIMER_H__ */
/** @} */

