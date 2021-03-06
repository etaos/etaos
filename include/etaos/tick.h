/*
 *  ETA/OS - Tick header
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

/** @file etaos/tick.h */

/**
 * @addtogroup tm
 * @{
 */

#ifndef __TICK_H__
#define __TICK_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/time.h>

/**
 * @brief Get the system tick.
 *
 * The sys_tick variable gets the tick count of the system clock and returns
 * it to the user.
 */
#define sys_tick clocksource_get_tick(sys_clk)

CDECL

extern void systick_setup(int irq, struct clocksource *src);
extern struct clocksource *sys_clk;

/**
 * @brief Get the system tick in seconds.
 * @return The system tick in seconds.
 */
static inline time_t tick_get_seconds(void)
{
	time_t now = (time_t)sys_tick;
	return (time_t)(now / 1000);
}

/**
 * @brief Get the system clock.
 * @return The system clock data pointer.
 * @note The system clock has to be set first using sysctl.
 */
static inline struct clocksource *sys_get_clock(void)
{
	return sys_clk;
}

CDECL_END


/**
 * @brief Calculate if a certain time unit has passed.
 * @param x Time 1.
 * @param y Time 2.
 *
 * Calculates if \p y is after \p x.
 */
#define time_after(x, y) ((x) > (y))
/**
 * @brief Calculate if a \p b is before \p a.
 * @param a Time 1.
 * @param b Time 2.
 */
#define time_before(a, b) ((a) < (b))
/**
 * Calculate if \p x is the same as or later than \p y.
 * @param x Time stamp 1.
 * @param y Time stamp 2.
 */
#define time_at_or_after(x, y) ((x) >= (y))

#endif /* __TICK_H__ */

/** @} */

