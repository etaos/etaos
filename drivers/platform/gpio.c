/*
 *  ETA/OS - Platform GPIO driver
 *  Copyright (C) 2015   Michel Megens <dev@michelmegens.net>
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/gpio.h>
#include <etaos/platform.h>

int pgpio_write_pin(int pin, int val)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_write(gpio, val);
}

int pgpio_pin_request(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_request(gpio);
}

int pgpio_pin_release(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_release(gpio);
}

int pgpio_read_pin(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_read(gpio);
}

int pgpio_direction_output(int pin, int val)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_direction_output(gpio, val);
}

int pgpio_direction_input(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_direction_input(gpio);
}

