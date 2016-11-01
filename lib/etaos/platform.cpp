/*
 *  ETA/OS - Platform implementation
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/gpio.h>
#include <etaos/platform.h>

#include <etaos/stl/platform.h>

/**
 * @brief Write to a platform pin.
 * @param pin Pin to write to.
 * @param value Value to write to \p pin.
 */
void Platform::pin_write(unsigned short pin, int& value)
{
	if(pgpio_pin_request(pin))
		return;

	pgpio_write_pin(pin, value);
	pgpio_pin_release(pin);
}

/**
 * @brief Read from a platform pin.
 * @param pin Pin to read from.
 * @return The value read from \p pin.
 */
int Platform::pin_read(unsigned short pin)
{
	int rv;

	if(pgpio_pin_request(pin))
		return -EINVAL;

	rv = pgpio_read_pin(pin);
	pgpio_pin_release(pin);

	return rv;
}

/**
 * @brief Change the direction of a platform pin to output.
 * @param pin Pin to set to output.
 * @param value Value to set on \p pin.
 */
void Platform::pin_out(unsigned short pin, int& value)
{
	if(pgpio_pin_request(pin))
		return;

	pgpio_direction_output(pin, value);
	pgpio_pin_release(pin);
}

/**
 * @brief Change the direction of a platform pin to input.
 * @param pin Pin to change to input.
 */
void Platform::pin_in(unsigned short pin)
{
	if(pgpio_pin_request(pin))
		return;

	pgpio_direction_input(pin);
	pgpio_pin_release(pin);
}

