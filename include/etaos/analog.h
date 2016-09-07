/*
 *  ETA/OS - Analog IO driver
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

#ifndef __ANALOG_H__
#define __ANALOG_H__

#include <etaos/kernel.h>
#include <etaos/device.h>

struct analog_pin {
	struct analog_chip *chip;
	uint8_t num;
};

struct analog_chip {
	struct analog_pin *pins; //!< Analog pin descriptor array.
	int num; //!< Length of the \p pins array.
	struct analog_pin *selected_pin;
	struct device dev;

	int (*get)(struct analog_pin *pin);
	int (*ioctl)(struct analog_chip *chip, unsigned long reg, void *arg);
};

typedef enum {
	ANALOG_SELECT_PIN,
} analog_ioctl_t;

extern struct analog_chip *analog_syschip;

#endif

