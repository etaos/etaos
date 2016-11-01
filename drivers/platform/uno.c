/*
 *  ETA/OS - Arduino UNO
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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
#include <etaos/types.h>
#include <etaos/gpio.h>
#include <etaos/platform.h>

#include <asm/pgm.h>

static const uint8_t __pgm platform_port_to_gpio[] = {
	15, 16, 17, 18, 19, 20, 21, 22, /* PORTD */
	0, 1, 2, 3, 4, 5,
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

