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

#ifndef __DHT_STL_H__
#define __DHT_STL_H__

/**
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/gpio.h>
#include <etaos/dht11.h>

/**
 * @brief STL wrapper class for the DHT device driver.
 */
class DHT
{
public:
	explicit DHT(struct gpio_pin *pin, dht11_ioctl_t mode = DHT_MODE_DHT11);
	virtual ~DHT();

	float readHumidity();
	float readTemperature();

private:
	int fd;
};

/** @} */

#endif
