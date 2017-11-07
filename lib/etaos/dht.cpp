/*
 *  ETA/OS - ETA/OS STL DHT
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
#include <etaos/device.h>
#include <etaos/stdio.h>
#include <etaos/gpio.h>
#include <etaos/math.h>
#include <etaos/panic.h>

#include <etaos/stl/dht11.h>

#include <asm/pgm.h>

/**
 * @brief Create a new DHT object.
 * @param pin Data I/O pin.
 * @param mode Device mode.
 */
DHT::DHT(struct gpio_pin *pin, dht11_ioctl_t mode)
{
	fd = open("/dev/dht11", _FDEV_SETUP_RW);
	if(fd < 0)
		panic_P(PSTR("Couldn't open DHT device!\n"));

	ioctl(filep(fd), mode, NULL);
	ioctl(filep(fd), DHT_SET_PIN, pin);
}

/**
 * @brief Destroy a DHT object.
 * @note This destructor will close the link to `/dev/dht11`.
 */
DHT::~DHT()
{
	if(this->fd < 0)
		return;

	close(this->fd);
}

/**
 * @brief Read the humidity from the DHT sensor.
 * @return The humidity read.
 * @retval NAN if an error occurred.
 */
float DHT::readHumidity()
{
	float f = NAN;

	ioctl(filep(fd), DHT_MEASURE_HUMIDITY, NULL);
	read(fd, &f, sizeof(f));
	return f;
}

/**
 * @brief Read the temperature from the DHT sensor.
 * @return The temperature read.
 * @retval NAN if an error occurred.
 */
float DHT::readTemperature()
{
	float f = NAN;

	ioctl(filep(fd), DHT_MEASURE_TEMPERATURE, NULL);
	read(fd, &f, sizeof(f));
	return f;
}
