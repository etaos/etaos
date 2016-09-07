/*
 *  ETA/OS - LM35 temperature sensor
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
#include <etaos/analog.h>
#include <etaos/lm35.h>
#include <etaos/vfs.h>
#include <etaos/stdio.h>

static struct analog_chip *lm35_chip;

float lm35_read(int pin)
{
	struct vfile *dev;
	int fd;
	float rv;

	if(!lm35_chip)
		lm35_set_analog_device(NULL);

	fd = open(analog_chip_to_name(lm35_chip), _FDEV_SETUP_RW);

	if(fd < 0)
		return (float)fd;

	dev = filep(fd);
	ioctl(dev, ANALOG_SELECT_PIN, &pin);
	rv = getc(dev);
	close(fd);

	return rv;
}

void lm35_set_analog_device(struct analog_chip *chip)
{
	if(!chip)
		lm35_chip = analog_syschip;
	else
		lm35_chip = chip;
}

