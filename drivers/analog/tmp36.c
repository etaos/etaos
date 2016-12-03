/*
 *  ETA/OS - TMP36 temperature sensor
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
 * @addtogroup tmp36
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdlib.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/analog.h>
#include <etaos/tmp36.h>

#include <asm/io.h>

static struct analog_chip *tmp36_chip;

/**
 * @brief Read the TMP36 sensor.
 * @param pin Analog pin which is connected to the TMP36.
 * @return The temperature value read.
 * @retval -9999.9999 in case of an error.
 */
float tmp36_read(int pin)
{
	struct file *file;
	int fd;
	float temperature;
	char analog_name[20] = "/dev";

	if(!tmp36_chip)
		tmp36_set_analog_device(NULL);

	strcat(analog_name, analog_chip_to_name(tmp36_chip));
	fd = open(analog_chip_to_name(analog_syschip), _FDEV_SETUP_RW);

	if(fd < 0)
		return -9999.9999F;

	file = filep(fd);
	ioctl(file, ANALOG_SELECT_PIN, &pin);
	temperature = fgetc(file);
	close(fd);

	temperature *= 5000.0f / 1024.0f;
	temperature -= 500.0f;
	temperature /= 10.0f;

	return temperature;
}

/**
 * @brief Set the analog device for the TMP36.
 * @param chip Analog chip to which the TMP36 is connected.
 * @note The TMP36 driver defaults to the #analog_syschip.
 */
void tmp36_set_analog_device(struct analog_chip *chip)
{
	if(!chip)
		tmp36_chip = analog_syschip;
	else
		tmp36_chip = chip;
}

/** @} */

