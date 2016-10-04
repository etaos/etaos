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

/**
 * @addtogroup hrtimer
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/panic.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/clocksource.h>
#include <etaos/hrtimer.h>

/**
 * @brief The system high resolution clock source.
 */
struct clocksource *hr_sys_clk;

static void hrtimer_source_add(struct hrtimer_source *src, struct hrtimer *timer)
{
	struct hrtimer *carriage;

	_raw_spin_lock(&src->base.lock);
	for(carriage = src->timers; carriage; carriage = carriage->next) {
		if(timer->ticks < carriage->ticks) {
			carriage->ticks -= timer->ticks;
			break;
		}

		timer->ticks -= carriage->ticks;
		timer->prev = carriage;
	}

	timer->next = carriage;

	if(timer->next)
		timer->next->prev = timer;
	if(timer->prev)
		timer->prev->next = timer;
	else
		src->timers = timer;
	_raw_spin_unlock(&src->base.lock);
}

static void hrtimer_source_delete(struct hrtimer_source *src, struct hrtimer *timer)
{
	if(timer->prev)
		timer->prev->next = timer->next;
	else
		src->timers = timer->next;
	if(timer->next) {
		timer->next->prev = timer->prev;
		timer->next->ticks += timer->ticks;
	}
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
	struct hrtimer_source *hrsrc;
	unsigned long resolution;

	timer = kzalloc(sizeof(*timer));
	hrsrc = container_of(src, struct hrtimer_source, base);

	if(!timer)
		panic_P("No memory available\n");

	/*
	 * Alternatively:
	 * ticks = (ns * src->freq) / 1E9;
	 */
	resolution = (1.0f/src->freq) * 1E9;
	timer->ticks = ns / resolution;
	timer->handle = handle;
	timer->handle_argument = arg;
	timer->base = hrsrc;

	if(test_bit(HRTIMER_ONESHOT, &flags))
		timer->timer_once = 0UL;
	else
		timer->timer_once = timer->ticks;

	hrtimer_source_add(hrsrc, timer);
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
	struct clocksource *cs;

	if(!timer || timer == SIGNALED)
		return -EINVAL;

	cs = hrtimer_to_source(timer);

	_raw_spin_lock(&cs->lock);
	timer->handle = NULL;
	hrtimer_source_delete(timer->base, timer);
	_raw_spin_unlock(&cs->lock);
	kfree(timer);

	return -EOK;
}

static void hrtimer_handle(struct hrtimer_source *cs)
{
	unsigned int diff;
	struct hrtimer *timer;

	diff = clocksource_update(&cs->base);
	if(!cs->timers || diff < 0 || diff == 0)
		return;

	while(cs->timers && diff) {
		timer = cs->timers;

		if(diff < timer->ticks) {
			timer->ticks -= diff;
			diff = 0;
		} else {
			diff -= timer->ticks;
			timer->ticks = 0;
		}

		if(timer->ticks == 0) {
			if(timer->handle)
				timer->handle(timer, timer->handle_argument);

			cs->timers = cs->timers->next;
			if(cs->timers)
				cs->timers->prev = NULL;

			if((timer->ticks = timer->timer_once) == 0)
				kfree(timer);
			else
				hrtimer_source_add(cs, timer);
		}
	}
}

/**
 * @brief High resolution timer interrupt.
 * @param data IRQ data.
 * @param arg IRQ argument (clocksource).
 */
irqreturn_t hrtimer_tick(struct irq_data *data, void *arg)
{
	struct clocksource *src;
	struct hrtimer_source *hrsrc;

	src = arg;
	hrsrc = container_of(src, struct hrtimer_source, base);
	
	timer_source_inc(src);
	hrtimer_handle(hrsrc);
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

