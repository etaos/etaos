/*
 *  ETA/OS - Time core
 *  Copyright (C) 2014, 2015   Michel Megens
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
 * @file kernel/time/timer.c
 * @addtogroup tm
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/error.h>
#include <etaos/time.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/spinlock.h>
#include <etaos/atomic.h>

/**
 * @brief Start a new timer.
 * @param timer Timer to start.
 */
static void timer_start_timer(struct timer *timer)
{
	struct clocksource *cs;

	cs = timer->source;
	clocksource_add_timer(cs, timer);
}

/**
 * @brief Create a new timer.
 * @param cs Clock source handling the timer.
 * @param ms Timer interval in miliseconds.
 * @param handle Timer handle.
 * @param arg Private timer data (passed to handle on trigger).
 * @param flags Timer flags.
 * @return The created time, NULL if no timer was created.
 */
struct timer *timer_create_timer(struct clocksource *cs, unsigned long ms,
		void (*handle)(struct timer*,void*), void *arg,
		unsigned long flags)
{
	struct timer *timer;

	if((timer = kzalloc(sizeof(*timer))) == NULL)
		return NULL;

	timer->tleft = (cs->freq / 1000UL) * ms;

	if(test_bit(TIMER_ONESHOT_FLAG, &flags))
		timer->ticks = 0;
	else
		timer->ticks = timer->tleft;

	timer->tleft += clocksource_get_diff(cs);
	timer->source = cs;
	timer->handle = handle;
	timer->priv_data = arg;

	timer_start_timer(timer);
	return timer;
}	

/**
 * @brief Stop a timer.
 * @param timer Timer to stop.
 * @return Error code.
 * @retval 0 on success.
 * @retval 1 if no timer was stopped.
 */
int timer_stop_timer(struct timer *timer)
{
	unsigned long flags;
	struct clocksource *cs = timer->source;

	if(!timer || timer == SIGNALED)
		return -1;

	raw_spin_lock_irqsave(&cs->lock, flags);
	timer->ticks = 0;
	timer->handle = NULL;
	timer->tleft = 0;
	raw_spin_unlock_irqrestore(&cs->lock, flags);

	if(timer->tleft) {
		clocksource_delete_timer(cs, timer);
		timer_start_timer(timer);
	}

	return 0;
}

extern void raw_clocksource_add_timer(struct clocksource *cs, struct timer *timer);
/**
 * @brief Process a given clock source and its timers.
 * @param cs Clock source to process.
 * @param diff Time difference in ticks since last call to this function.
 * @warning This function should NOT be called by applications.
 */
void timer_process_clock(struct clocksource *cs, unsigned int diff)
{
	struct timer *timer;
	
	_raw_spin_lock(&cs->lock);
	if(!cs->thead || diff < 0 || diff == 0) {
		_raw_spin_unlock(&cs->lock);
		return;
	}

	while(cs->thead && diff) {
		timer = cs->thead;

		if(diff < timer->tleft) {
			timer->tleft -= diff;
			diff = 0;
		} else {
			diff -= timer->tleft;
			timer->tleft = 0;
		}

		if(timer->tleft == 0) {
			if(timer->handle) {
				timer->handle(timer, timer->priv_data);
			}

			cs->thead = cs->thead->next;
			if(cs->thead)
				cs->thead->prev = NULL;

			if((timer->tleft = timer->ticks) == 0) {
				kfree(timer);
			} else {
				raw_clocksource_add_timer(cs, timer);
			}
		}
	}
	_raw_spin_unlock(&cs->lock);
}


/** @} */
