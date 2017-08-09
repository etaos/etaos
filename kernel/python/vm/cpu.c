/*
 *  ETA/OS - Python CPU library
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
 *  Copyright (C) 2017   Dean Hall
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

#undef __FILE_ID__
#define __FILE_ID__ 0x1A

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/python.h>
#include <etaos/gpio.h>
#include <etaos/analog.h>

#if defined(CONFIG_GPIO) || defined(CONFIG_GPIO_MODULE)
bool pm_cpu_pin_read(int32_t _pin)
{
	struct gpio_pin *pin;
	bool value;

	pin = gpio_chip_to_pin(gpio_sys_chip, (uint16_t)_pin);
	if(gpio_pin_request(pin) != -EOK)
		return false;

	value = gpio_pin_read(pin) != 0;

	gpio_pin_release(pin);
	return value;
}

void pm_cpu_direction_input(int32_t _pin)
{
	struct gpio_pin *pin;

	pin = gpio_chip_to_pin(gpio_sys_chip, (uint16_t)_pin);
	if(gpio_pin_request(pin) != -EOK)
		return;

	gpio_direction_input(pin);
	gpio_pin_release(pin);
}

void pm_cpu_direction_output(int32_t _pin, bool value)
{
	struct gpio_pin *pin;

	pin = gpio_chip_to_pin(gpio_sys_chip, (uint16_t)_pin);
	if(gpio_pin_request(pin) != -EOK)
		return;

	gpio_direction_output(pin, value);
	gpio_pin_release(pin);
}

void pm_cpu_pin_write(int32_t _pin, bool value)
{
	struct gpio_pin *pin;

	pin = gpio_chip_to_pin(gpio_sys_chip, (uint16_t)_pin);
	if(gpio_pin_request(pin) != -EOK)
		return;

	gpio_pin_write(pin, value);
	gpio_pin_release(pin);
}
#endif

#if defined(CONFIG_ANALOG) || defined(CONFIG_ANALOG_MODULE)
float pm_cpu_analog_pin_read(int32_t _pin)
{
	int __pin = (int)_pin & 0x7FF;
	struct analog_chip *chip = analog_syschip;
	struct analog_pin *pin;

	if(__pin >= chip->num)
		return 0.0f;

	pin = &chip->pins[__pin];
	return (float)analog_read(pin);
}
#endif

