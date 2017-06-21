/*
 *  ETA/OS - Python CPU library
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

#ifndef __PMCPU_H__
#define __PMCPU_H__

#include <etaos/types.h>

CDECL
extern bool pm_cpu_pin_read(int32_t pin);
extern void pm_cpu_direction_input(int32_t pin);
extern void pm_cpu_direction_output(int32_t pin, bool value);
extern void pm_cpu_pin_write(int32_t pin, bool value);
extern float pm_cpu_analog_pin_read(int32_t _pin);
CDECL_END

#endif
