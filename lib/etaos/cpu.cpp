/*
 *  ETA/OS - ETA/OS STL CPU
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

/**
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/gpio.h>
#include <etaos/analog.h>

#include <etaos/stl/cpu.h>

/**
 * @brief Write a value to a pin.
 * @param pin Pin to write to.
 * @param value Value to set on \p pin.
 */
void CPU::pin_write(unsigned short pin, int& value)
{
	struct gpio_pin *p = gpio_chip_to_pin(gpio_sys_chip, pin);

	if(gpio_pin_request(p))
		return;

	gpio_pin_write(p, value);
	gpio_pin_release(p);
}

/**
 * @brief Read from a GPIO pin.
 * @param pin Pin to read from.
 * @return The value read from \p pin.
 */
int CPU::pin_read(unsigned short pin)
{
	int rv;
	struct gpio_pin *p = gpio_chip_to_pin(gpio_sys_chip, pin);

	if(gpio_pin_request(p))
		return -EINVAL;

	rv = gpio_pin_read(p);
	gpio_pin_release(p);

	return rv;
}

/**
 * @brief Set a GPIO pin to input.
 * @param pin Pin to set to input.
 */
void CPU::pin_in(unsigned short pin)
{
	struct gpio_pin *p = gpio_chip_to_pin(gpio_sys_chip, pin);

	if(gpio_pin_request(p))
		return;

	gpio_direction_input(p);
	gpio_pin_release(p);
}

/**
 * @brief Set a GPIO pin to output.
 * @param pin Pin to set to output.
 * @param value Value to set on pin.
 */
void CPU::pin_out(unsigned short pin, int& value)
{
	struct gpio_pin *p = gpio_chip_to_pin(gpio_sys_chip, pin);

	if(gpio_pin_request(p))
		return;

	gpio_direction_output(p, value);
	gpio_pin_release(p);
}

/**
 * @brief Read from the ADC.
 * @param pin Analog pin to read from.
 * @return The analog value read from \p pin.
 */
int CPU::adc_read(unsigned short pin)
{
	int fd, rv;
	struct vfile *file;
	const char *name = analog_chip_to_name(analog_syschip);

	fd = open(name, _FDEV_SETUP_RW);
	if(fd < 0)
		return -EINVAL;

	file = filep(fd);
	ioctl(file, ANALOG_SELECT_PIN, &pin);
	rv = getc(file);
	close(fd);

	return rv;
}

/** @} */

