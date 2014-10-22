/*
 *  ETA/OS - Time core
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
 * @file kernel/time/time.c
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
#include <etaos/string.h>
#include <etaos/atomic.h>

static struct list_head sources = STATIC_INIT_LIST_HEAD(sources);

/**
 * @brief initialise a new clock source.
 * @param name Name of the clocksource.
 * @param cs Clock source which has to be initialised.
 * @param freq Frequency of the clock source.
 * @param enable Function pointer to enable the source.
 * @param disable Function pointer to disable the source.
 */
int tm_clock_source_initialise(const char *name, struct clocksource *cs,
		unsigned long freq, int (*enable)(struct clocksource *cs),
		void (*disable)(struct clocksource *cs))
{
	if(!cs || !name)
		return -EINVAL;

	cs->thead = NULL;
	cs->name = name;
	cs->freq = freq;
	cs->enable = enable;
	cs->disable = disable;
	atomic64_init(&cs->tc);
	cs->tc_resume = 0LL;
	spin_lock_init(&cs->lock);
	list_add(&cs->list, &sources);
	return -EOK;
}

/**
 * @brief Start a new timer.
 * @param timer Timer to start.
 */
static void tm_start_timer(struct timer *timer)
{
	struct clocksource *cs;
	struct timer *_timer;

	cs = timer->source;

	raw_spin_lock(&cs->lock);
	for(_timer = cs->thead; _timer; _timer = _timer->next) {
		if(timer->tleft < _timer->tleft) {
			_timer->tleft -= timer->tleft;
			break;
		}

		timer->tleft -= _timer->tleft;
		timer->prev = _timer;
	}

	timer->next = _timer;
	if(timer->next)
		timer->next->prev = timer;
	if(timer->prev)
		timer->prev->next = timer;
	else
		cs->thead = timer;
	raw_spin_unlock(&cs->lock);
}

/**
 * @brief Calculate the clockskew of a clocksource.
 * @param cs Clocksource to calculate the clockskew of.
 * @return The clockskew of \p cs.
 */
static unsigned int cs_get_diff(struct clocksource *cs)
{
	return (unsigned int)(atomic64_get(&cs->tc) - cs->tc_resume);
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
struct timer *tm_create_timer(struct clocksource *cs, unsigned long ms,
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

	timer->tleft += cs_get_diff(cs);
	timer->source = cs;
	timer->handle = handle;
	timer->priv_data = arg;

	tm_start_timer(timer);
	return timer;
}	

/**
 * @brief Stop a timer.
 * @param timer Timer to stop.
 * @return Error code.
 * @retval 0 on success.
 * @retval 1 if no timer was stopped.
 */
int tm_stop_timer(struct timer *timer)
{
	struct clocksource *cs;

	if(!timer || timer == SIGNALED)
		return -1;

	timer->ticks = 0;
	timer->handle = NULL;
	cs = timer->source;

	if(timer->tleft) {
		raw_spin_lock(&cs->lock);
		if(timer->prev)
			timer->prev->next = timer->next;
		else
			cs->thead = timer->next;
		if(timer->next) {
			timer->next->prev = timer->prev;
			timer->next->tleft += timer->tleft;
		}
		raw_spin_unlock(&cs->lock);
		timer->tleft = 0;
		tm_start_timer(timer);
	}

	return 0;
}

/**
 * @brief Update the clockskew of a clocksource.
 * @param source Clocksource to update.
 * @return The clockskew of \p source.
 * @note You should call tm_process_clock as soon as possible after calling
 *       this function to keep timers up to date.
 */
unsigned int tm_update_source(struct clocksource *source)
{
	unsigned int diff;

	diff = cs_get_diff(source);
	source->tc_resume = atomic64_get(&source->tc);
	return diff;
}

/**
 * @brief Process a given clock source and its timers.
 * @param cs Clock source to process.
 * @param diff Time difference in ticks since last call to this function.
 * @warning This function should NOT be called by applications.
 */
void tm_process_clock(struct clocksource *cs, unsigned int diff)
{
	struct timer *timer;
	
	if(!cs->thead || diff < 0 || diff == 0)
		return;

	raw_spin_lock(&cs->lock);
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
				raw_spin_unlock(&cs->lock);
				timer->handle(timer, timer->priv_data);
				raw_spin_lock(&cs->lock);
			}

			cs->thead = cs->thead->next;
			if(cs->thead)
				cs->thead->prev = NULL;
			if((timer->tleft = timer->ticks) == 0)
				kfree(timer);
			else {
				raw_spin_unlock(&cs->lock);
				tm_start_timer(timer);
				raw_spin_lock(&cs->lock);
			}
		}
	}
	raw_spin_unlock(&cs->lock);
}

/**
 * @brief Get a timer source based on its name.
 * @param name Name to search for.
 * @return The clock source, if found.
 * @retval NULL if no clocksource was found.
 */
struct clocksource *tm_get_source_by_name(const char *name)
{
	struct clocksource *cs;
	struct list_head *carriage;

	list_for_each(carriage, &sources) {
		cs = list_entry(carriage, struct clocksource, list);
		if(!strcmp(name, cs->name))
				return cs;
	}

	return NULL;
}

/** @} */
