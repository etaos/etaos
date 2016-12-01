/*
 *  ETA/OS - TMP36 temperature sensor
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

#ifndef __TMP36GZ_H__
#define __TMP36GZ_H__

#include <etaos/kernel.h>
#include <etaos/analog.h>

CDECL
extern float tmp36_read(int pin);
extern void tmp36_set_analog_device(struct analog_chip *chip);
CDECL_END

#endif

