/*
 *  ETA/OS - Timer header
 *  Copyright (C) 2014, 2015   Michel Megens
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
 * @file etaos/time.h Time header.
 * @addtogroup tm
 * @{
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/spinlock.h>
#include <etaos/atomic.h>
#include <etaos/clocksource.h>

/**
 * @struct timer
 * @brief Describes a single (virtual) timer.
 */
struct timer {
	struct list_head entry;
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
	time_t expire_at;
	unsigned int interval;
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
 * @def cs_last_update
 * @brief Get the system tick of the last moment \p __cs was updated.
 * @param __cs Clocksource to get the last update timestamp from.
 */
#define cs_last_update(__cs) __cs->tc_update

#define NEVER -1

CDECL
/**
 * @brief Safely increment the tick count of a clock source.
 * @param cs Clock source which tick count is to be incremented.
 */
static inline void timer_source_inc(struct clocksource *cs)
{
	cs->count += 1ULL;
}

extern struct clocksource *timer_get_source_by_name(const char *name);

extern void timer_process(struct clocksource *cs);
extern struct timer *timer_create(struct clocksource *cs, unsigned long ms,
		void (*handle)(struct timer*,void*), void *arg,
		unsigned long flags);
extern int timer_stop(struct timer *timer);
extern int raw_timer_stop(struct timer *timer);

CDECL_END

#endif /* __TIMER_H__ */
/** @} */

