/*
 *  ETA/OS - Watchdog driver
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdlib.h>

#define watchdog_reset() __asm__ __volatile__("wdr")

#define WDT_15MS  0
#define WDT_30MS  1
#define WDT_60MS  2
#define WDT_120MS 3
#define WDT_250MS 4
#define WDT_500MS 5
#define WDT_1S    6

struct watchdog {
	bool enabled;
	short ms_delay;
};

extern struct watchdog wdt;

#ifdef CONFIG_WATCHDOG
CDECL
static inline struct watchdog *watchdog_get_info(void)
{
	return &wdt;
}

extern void watchdog_enable(const uint8_t value);
extern void watchdog_disable(void);
#else
#define watchdog_get_info()
#define watchdog_enable()
#define watchdog_disable()
#endif
CDECL_END

#endif /* __WATCHDOG_H__ */
