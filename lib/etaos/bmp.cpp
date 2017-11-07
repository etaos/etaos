/*
 *  ETA/OS - ETA/OS STL BMP085
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
#include <etaos/bmp085.h>
#include <etaos/panic.h>
#include <etaos/stdio.h>
#include <etaos/math.h>

#include <etaos/stl/bmp085.h>

#include <asm/pgm.h>

/**
 * @brief Create a new BMP085 object.
 */
BMP085::BMP085()
{
	fd = open("/dev/bmp085", _FDEV_SETUP_RW);
	if(fd < 0)
		panic_P(PSTR("Couldn't open BMP device!\n"));

	ioctl(filep(fd), BMP_MEASURE_PRESSURE, NULL);
}

/**
 * @brief Destroy a BMP085 object.
 *
 * This destructor will close the link to `/dev/bmp085`.
 */
BMP085::~BMP085()
{
	if(this->fd < 0)
		return;

	close(this->fd);
}

/**
 * @brief Read the pressure from the BMP085 sensor.
 * @return The pressure read.
 */
float BMP085::readPressure()
{
	float f = NAN;

	read(fd, &f, sizeof(f));
	return f;
}
