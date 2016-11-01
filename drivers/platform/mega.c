/*
 *  ETA/OS - Arduino MEGA
 *  Copyright (C) 2015   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/gpio.h>
#include <etaos/platform.h>

#include <asm/pgm.h>

static const uint8_t __pgm platform_port_to_gpio[] = {
	32, 33, 36, 37, 53, 35, 59, 60,
	61, 62, 12, 13, 14, 15, 65, 64,
	57, 56, 27, 26, 25, 24, 0,  1,
	2,  3,  4,  5,  6,  7,  23, 22,
	21, 20, 19, 18, 17, 16, 31, 50,
	49, 48, 87, 86, 85, 84, 83, 82,
	81, 80, 11, 10, 9,  8,
};

struct gpio_pin *platform_pin_to_gpio(int pin)
{
	uint8_t gpio;
	struct gpio_chip *chip;

	if(pin > 13)
			return NULL;

	gpio = pgm_read_byte(platform_port_to_gpio+pin);
	chip = gpio_sys_chip;
	return gpio_chip_to_pin(chip, gpio);
}

