/*
 *  ETA/OS - High resolution timers
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
 * @file include/etaos/hrtimer.h High resolution timers
 */

#ifndef __HR_TIMERS__
#define __HR_TIMERS__

/**
 * @addtogroup hrtimer
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/list.h>
#include <etaos/timer.h>
#include <etaos/clocksource.h>

/**
 * @brief High resolution timer structure.
 */
struct hrtimer {
	struct list_head entry;
	struct clocksource *source;

	/**
	 * @brief Handler for the timer.
	 * @param timer Reference to the timer.
	 * @param arg Handler argument.
	 */
	void (*handle)(struct hrtimer *timer, void *arg);
	void *handle_argument; //!< Argument passed to \p handle.
	time_t expire_at;
	unsigned long interval;
};

extern struct clocksource *hr_sys_clk;

#define HRTIMER_ONESHOT		0 //!< Run-once flag bit.

/**
 * @name High resolution timer flags
 * @{
 */
#define HRTIMER_ONESHOT_FLAG   BIT(HRTIMER_ONESHOT) //!< Run-once flag.
/** @} */

CDECL
/**
 * @brief Get the base clocksource of a hrtimer.
 * @param timer hrtimer to get the base clocksource for.
 * @return The base clocksource of \p timer.
 */
static inline struct clocksource *hrtimer_to_source(struct hrtimer *timer)
{
	return timer->source;
}

extern struct hrtimer *hrtimer_create(struct clocksource *src, uint64_t ns,
				void (*handle)(struct hrtimer *, void*),
				void *arg, unsigned long flags);
extern irqreturn_t hrtimer_tick(struct irq_data *data, void *arg);
extern void hrtimer_init(int irq, struct clocksource *src);
extern int hrtimer_stop(struct hrtimer *timer);
CDECL_END

/** @} */
#endif

