/*
 *  ETA/OS - High resolution timers
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

#ifndef __HR_TIMERS__
#define __HR_TIMERS__

#include <etaos/kernel.h>
#include <etaos/timer.h>
#include <etaos/clocksource.h>

struct hrtimer_source {
	struct clocksource base; //!< Base clocksource.
	struct hrtimer *timers; //!< Linked list head.
};

struct hrtimer {
	struct hrtimer *next, //!< List next pointer.
		       *prev; //!< List prev pointer.
	struct hrtimer_source *base; //!< Timer base.

	void (*handle)(struct hrtimer *timer, void *arg);
	void *handle_argument;

	unsigned long ticks,
		      timer_once;
};

extern struct clocksource *hr_sys_clk;

/**
 * @name High resolution timer flags
 * @{
 */
#define HRTIMER_ONESHOT		0 //!< Run-once flag.
/** @} */

CDECL
static inline void hrtimer_source_init(const char *name,
				       struct hrtimer_source *src,
				       int (*enable)(struct clocksource *),
				       void (*disable)(struct clocksource*),
				       unsigned long freq)
{
	src->timers = NULL;
	clocksource_init(name, &src->base, freq, enable, disable);
}
extern struct hrtimer *hrtimer_create(struct clocksource *src, uint64_t ns,
				void (*handle)(struct hrtimer *, void*),
				void *arg, unsigned long flags);
extern irqreturn_t hrtimer_tick(struct irq_data *data, void *arg);
CDECL_END

#endif
