/*
 *  ETA/OS - LM35 temperature sensor
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
 * @addtogroup lm35
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/analog.h>
#include <etaos/lm35.h>
#include <etaos/vfs.h>
#include <etaos/stdio.h>
#include <etaos/string.h>

static struct analog_chip *lm35_chip;

/**
 * @brief Read the temperature from the LM35 device.
 * @param pin Analog pin to read from.
 * @return The current temperature as measured by the LM35.
 */
float lm35_read(int pin)
{
	struct file *dev;
	int fd;
	float rv;
	char analog_name[20] = "/dev/";

	if(!lm35_chip)
		lm35_set_analog_device(NULL);

	strcat(analog_name, analog_chip_to_name(lm35_chip));
	fd = open(analog_name, _FDEV_SETUP_RW);

	if(fd < 0)
		return (float)fd;

	dev = filep(fd);
	ioctl(dev, ANALOG_SELECT_PIN, &pin);
	rv = getc(dev);
	close(fd);

	rv = ((rv / 1024.0) * 5000.0) / 10.0;
	return rv;
}

/**
 * @brief Set the analog the device the LM35 driver should use.
 * @param chip Analog chip to set.
 */
void lm35_set_analog_device(struct analog_chip *chip)
{
	if(!chip)
		lm35_chip = analog_syschip;
	else
		lm35_chip = chip;
}

/** @} */

