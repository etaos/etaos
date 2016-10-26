/*
 *  ETA/OS - Platform implementation
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/gpio.h>
#include <etaos/platform.h>

#include <etaos/stl/platform.h>

void Platform::pin_write(unsigned short pin, int& value)
{
	if(pgpio_pin_request(pin))
		return;

	pgpio_write_pin(pin, value);
	pgpio_pin_release(pin);
}

int Platform::pin_read(unsigned short pin)
{
	int rv;

	if(pgpio_pin_request(pin))
		return -EINVAL;

	rv = pgpio_read_pin(pin);
	pgpio_pin_release(pin);

	return rv;
}

void Platform::pin_out(unsigned short pin, int& value)
{
	if(pgpio_pin_request(pin))
		return;

	pgpio_direction_output(pin, value);
	pgpio_pin_release(pin);
}

void Platform::pin_in(unsigned short pin)
{
	if(pgpio_pin_request(pin))
		return;

	pgpio_direction_input(pin);
	pgpio_pin_release(pin);
}

