/*
 *  ETA/OS - Delay implementation
 *  Copyright (C) 2014, 2015   Michel Megens <dev@michelmegens.net>
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
#include <etaos/tick.h>
#include <etaos/delay.h>

#ifdef CONFIG_DELAY_US
/**
 * @brief Delay for a certain amount of micro seconds.
 * @param us Amount of micro seconds to delay.
 */
void delay_us(double us)
{
	arch_delay_us(us);
}
#endif

#ifdef CONFIG_DELAY_MS
/**
 * @brief Delay for a certain amount of miliseconds.
 * @param ms Miliseconds to delay.
 * @note Avoid usage if the scheduler is in use. Use sleep instead.
 * @see sleep
 */
void delay(unsigned int ms)
{
	int64_t count;

	count = sys_tick;
	while(true) {
		if(time_after(sys_tick, count + ms))
			return;
	}
}
#endif

/* @} */
