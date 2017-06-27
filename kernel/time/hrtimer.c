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
 * @addtogroup hrtimer
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/panic.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/clocksource.h>
#include <etaos/hrtimer.h>
#include <etaos/tick.h>

struct clocksource *hr_sys_clk;

static int hrtimer_list_comparator(struct list_head *lh1, struct list_head *lh2)
{
	struct hrtimer *t1, *t2;

	t1 = list_entry(lh1, struct hrtimer, entry);
	t2 = list_entry(lh2, struct hrtimer, entry);

	if(t1->expire_at < t2->expire_at)
		return -1;
	else if(t1->expire_at > t2->expire_at)
		return 1;

	return 0;
}

/**
 * @brief Create a new high resolution timer.
 * @param src Clock source to run the timer on.
 * @param ns Interval in nanoseconds.
 * @param handle Timer handler.
 * @param arg Argument to \p handle.
 * @param flags Timer flags.
 * @return The created hrtimer.
 */
struct hrtimer *hrtimer_create(struct clocksource *src, uint64_t ns,
				void (*handle)(struct hrtimer *, void*),
				void *arg, unsigned long flags)
{
	struct hrtimer *timer;

	timer = kzalloc(sizeof(*timer));

	if(!timer)
		panic_P("No memory available\n");

	timer->source = src;
	timer->interval = timer->expire_at = (ns * src->freq) / 1E9;
	timer->expire_at += clocksource_get_tick(src);
	timer->handle = handle;
	timer->handle_argument = arg;
	timer->source = src;
	list_head_init(&timer->entry);

	if(test_bit(HRTIMER_ONESHOT, &flags))
		timer->interval = 0;

	clocksource_insert_timer(src, &timer->entry, &hrtimer_list_comparator);
	return timer;
}

/**
 * @brief Stop a currently running hrtimer.
 * @param timer HR timer to stop.
 * @return An error code.
 * @retval -EINVAL if something went wrong.
 * @retval -EOK if the timer was succesfully stopped and deleted.
 */
int hrtimer_stop(struct hrtimer *timer)
{
	int rc;

	rc = clocksource_remove_timer(timer->source, &timer->entry);
	if(rc == -EOK)
		kfree(timer);

	return rc;
}

static inline bool hrtimer_is_expired(struct hrtimer *timer)
{
	time_t now;

	now = clocksource_get_tick(timer->source);
	return time_at_or_after(now, timer->expire_at);
}

static void hrtimer_handle(struct clocksource *cs)
{
	unsigned long flags;
	struct list_head *carriage, *x;
	struct hrtimer *timer;

	raw_spin_lock_irqsave(&cs->lock, flags);
	clocksource_update(cs);

	list_for_each_safe(carriage, x, &cs->timers) {
		timer = list_entry(carriage, struct hrtimer, entry);
		if(hrtimer_is_expired(timer)) {
			list_del(carriage);
			
			if(timer->handle)
				timer->handle(timer, timer->handle_argument);

			if(timer->interval) {
				timer->expire_at = timer->interval +
					clocksource_get_tick(cs);
				raw_clocksource_insert_timer(cs, &timer->entry,
						&hrtimer_list_comparator);
			} else {
				kfree(timer);
			}

		}
	}

	raw_spin_unlock_irqrestore(&cs->lock, flags);
}

/**
 * @brief High resolution timer interrupt.
 * @param data IRQ data.
 * @param arg IRQ argument (clocksource).
 * @return IRQ handle status.
 * @retval IRQ_HANDLED
 */
irqreturn_t hrtimer_tick(struct irq_data *data, void *arg)
{
	struct clocksource *src;

	src = arg;
	
	timer_source_inc(src);
	hrtimer_handle(src);
	return IRQ_HANDLED;
}

/**
 * @brief Initialise the hrtimer core.
 * @param irq HR timer IRQ number.
 * @param src HR timer clock source.
 */
void hrtimer_init(int irq, struct clocksource *src)
{
	irq_request(irq, &hrtimer_tick, IRQ_RISING_MASK, src);
}

/** @} */

