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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/error.h>
#include <etaos/time.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/spinlock.h>
#include <etaos/string.h>

static struct list_head sources = STATIC_INIT_LIST_HEAD(sources);

int tm_clock_source_initialise(const char *name, struct clocksource *cs,
		unsigned long freq, int (*enable)(struct clocksource *cs),
		void (*disable)(struct clocksource *cs))
{
	if(!cs || !name)
		return -EINVAL;

	list_head_init(&cs->timers);
	cs->name = name;
	cs->freq = freq;
	cs->enable = enable;
	cs->disable = disable;
	cs->tc = 0;
	cs->tc_resume = 0;
	spin_lock_init(&cs->lock);
	list_add(&cs->list, &sources);
	return -EOK;
}

static void tm_start_timer(struct timer *timer)
{
	struct list_head *carriage;
	struct clocksource *cs;
	struct timer *_timer, *prev_timer;

	cs = timer->source;
	prev_timer = NULL;

	spin_lock(&cs->lock);
	if(list_empty(&cs->timers)) {
		list_add(&timer->list, &cs->timers);
		spin_unlock(&cs->lock);
		return;
	}

	list_for_each(carriage, &cs->timers) {
		_timer = list_entry(carriage, struct timer, list);
		if(timer->tleft < _timer->tleft) {
			_timer->tleft -= timer->tleft;
			break;
		}

		timer->tleft -= _timer->tleft;
		prev_timer = _timer;
	}

	if(prev_timer)
		list_add(&timer->list, &prev_timer->list);
	else
		list_add(&timer->list, &cs->timers);
	spin_unlock(&cs->lock);
}

struct timer *tm_create_timer(struct clocksource *cs, unsigned long ms,
		void (*handle)(struct timer*,void*), void *arg,
		unsigned long flags)
{
	struct timer *timer;

	if((timer = kmalloc(sizeof(*timer))) == NULL)
		return NULL;

	timer->tleft = (cs->freq / 1000UL) * ms;
	
	if(test_bit(TIMER_ONESHOT_FLAG, &flags))
		timer->ticks = 0;
	else
		timer->ticks = timer->tleft;

	timer->source = cs;
	timer->tleft += cs->tc - cs->tc_resume;
	timer->handle = handle;
	timer->priv_data = arg;

	tm_start_timer(timer);
	return timer;
}	

int tm_stop_timer(struct timer *timer)
{
	struct clocksource *cs;
	struct timer *next;

	timer->ticks = 0;
	cs = timer->source;

	if(timer->tleft) {
		spin_lock(&cs->lock);
		if(!list_is_last(&timer->list, &cs->timers)) {
			next = list_next_entry(timer, list);
			next->tleft += timer->tleft;
		}

		list_del(&timer->list);
		spin_unlock(&cs->lock);
		kfree(timer);
		return 0;
	}

	return 1;
}

void tm_process_clock(struct clocksource *cs)
{
	struct list_head *carriage, *tmp;
	struct timer *timer;
	unsigned long diff;

	diff = cs->tc - cs->tc_resume;
	cs->tc_resume = cs->tc;
	
	if(list_empty(&cs->timers))
		return;

	spin_lock(&cs->lock);
	list_for_each_safe(carriage, tmp, &cs->timers) {
		timer = list_entry(carriage, struct timer, list);

		if(timer->tleft < diff) {
			diff -= timer->tleft;
			timer->tleft = 0;
		} else {
			timer->tleft -= diff;
			diff = 0;
		}

		if(!timer->tleft) {
			if(timer->handle) {
				spin_unlock(&cs->lock);
				timer->handle(timer, timer->priv_data);
				spin_lock(&cs->lock);
			}

			list_del(&timer->list);
			if(timer->ticks) {
				timer->tleft = timer->ticks;
				tm_start_timer(timer);
			} else {
				kfree(timer);
			}
		}

		if(!diff)
			break;
	}

	spin_unlock(&cs->lock);
}

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

