/*
 *  ETA/OS - AVR PWM timers
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

#ifndef __PWM_AVR_H__
#define __PWM_AVR_H__

#include <etaos/kernel.h>

#define PWM_USE_OC_PINS 8

CDECL
extern struct clocksource *pwm_get_clock0(void);
extern struct clocksource *pwm_get_clock1(void);
extern struct clocksource *pwm_get_clock2(void);
extern struct clocksource *pwm_get_clock3(void);
CDECL_END

#endif
