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

#ifndef __TIMER_H__
#define __TIMER_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/atomic.h>
#include <etaos/spinlock.h>

struct clocksource {
	const char *name;

	int (*enable)(struct clocksource*);
	void (*disable)(struct clocksource*);
	unsigned long freq;
	atomic64_t tc;
	int64_t tc_resume;
	spinlock_t lock;

	struct list_head list;
	struct list_head timers;
};

struct timer {
	struct list_head list;

	struct clocksource *source;
	void (*handle)(struct timer *timer, void *arg);
	void *priv_data;

	unsigned long tleft;
	unsigned long ticks;
};

#define TIMER_ONESHOT_FLAG 0
#define TIMER_ONESHOT_MASK (1<<TIMER_ONESHOT_FLAG)


extern int64_t tm_update_source(struct clocksource *source);
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

