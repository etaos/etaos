/*
 *  ETA/OS - Platform GPIO driver
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
#include <etaos/types.h>
#include <etaos/gpio.h>
#include <etaos/platform.h>

/**
 * @brief Write a value to a platform pin.
 * @param pin pin to write to.
 * @param val value to write.
 * @return Error code.
 * @retval -EOK success.
 * @retval -EINVAL on error.
 */
int pgpio_write_pin(int pin, int val)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_write(gpio, val);
}

/**
 * @brief Request a platform GPIO pin.
 * @param pin Pin to request.
 * @return Error code.
 * @retval -EINVAL pin could not be requested.
 * @retval -EOK on success.
 */
int pgpio_pin_request(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_request(gpio);
}

/**
 * @brief Release a requested platform pin.
 * @param pin Pin to release.
 * @return Error code.
 * @retval 1 if the pin was never requested.
 * @retval -EOK on success.
 */
int pgpio_pin_release(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_release(gpio);
}

/**
 * @brief Read the value of a platform pin.
 * @param pin Pin to read from.
 * @return The value read from the pin.
 * @retval -EINVAL on error.
 */
int pgpio_read_pin(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_pin_read(gpio);
}

/**
 * @brief Set the direction of a platform pin to output.
 * @param pin Pin to configure as output.
 * @param val Value to write to the output pin.
 * @return Error code.
 * @retval -EOK success.
 * @retval -EINVAL on error.
 */
int pgpio_direction_output(int pin, int val)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_direction_output(gpio, val);
}

/**
 * @brief Set a platform pin as input pin.
 * @param pin Pin to configure as input.
 * @return Error code.
 * @retval -EOK success.
 * @retval -EINVAL on error.
 */
int pgpio_direction_input(int pin)
{
	struct gpio_pin *gpio;

	gpio = platform_pin_to_gpio(pin);
	if(!gpio)
		return -EINVAL;

	return gpio_direction_input(gpio);
}

