/*
 *  ETA/OS - Timer header
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

#ifndef CLOCKSOURCE_H
#define CLOCKSOURCE_H

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/spinlock.h>

struct timer;
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

	volatile tick_t count; //!< Tick counter.
	tick_t tc_update; //!< Timer update value.

	spinlock_t lock; //!< Clocksource lock.

	struct list_head list; //!< List of clocksources.
	struct list_head timers; //!< Timer list head.
	struct timer *thead; //!< Timer list head.
};

CDECL
/**
 * @brief Get the tick count from a clock source.
 * @param cs Clock source which tick count is requested.
 * @return The struct clocksource::count attribute of \p cs.
 */
static inline tick_t clocksource_get_tick(struct clocksource *cs)
{
	tick_t rv;
	unsigned long flags;

	irq_save_and_disable(&flags);
	rv = cs->count;
	irq_restore(&flags);

	return rv;
}

/**
 * @brief Calculate the clockskew of a clocksource.
 * @param cs Clocksource to calculate the clockskew of.
 * @return The clockskew of \p cs.
 */
static inline tick_t clocksource_get_diff(struct clocksource *cs)
{
	tick_t update;

	update = clocksource_get_tick(cs);
	update -= cs->tc_update;

	return update;
}

extern int clocksource_init(const char *name, struct clocksource *cs,
		unsigned long freq, int (*enable)(struct clocksource *cs),
		void (*disable)(struct clocksource *cs));
extern void clocksource_add_timer(struct clocksource *cs, struct timer *t);
extern void clocksource_delete_timer(struct clocksource *cs, struct timer *timer);
extern tick_t clocksource_update(struct clocksource *cs);
extern time_t clocksource_get_seconds(struct clocksource *src);

/* Timer rewrite funcs */

extern int raw_clocksource_remove_timer(struct list_head *lh);
extern void clocksource_insert_timer(struct clocksource *cs,
		struct list_head *lh, list_comparator_t comp);
extern void raw_clocksource_insert_timer(struct clocksource *cs,
		struct list_head *lh, list_comparator_t comp);
extern int clocksource_remove_timer(struct clocksource *cs,
		struct list_head *lh);
CDECL_END

#endif /* CLOCKSOURCE_H */

