/*
 *  ETA/OS - LM35 temperature sensor
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
 * @file include/etaos/lm35.h LM35 driver
 */

#ifndef __LM35_H__
#define __LM35_H__

/**
 * @addtogroup lm35
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>

CDECL
extern float lm35_read(int pin);
extern void lm35_set_analog_device(struct analog_chip *chip);
CDECL_END

/** @} */

#endif

