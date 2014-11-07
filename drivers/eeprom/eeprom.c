/*
 *  ETA/OS - EEPROM header
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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
#include <etaos/eeprom.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/device.h>
#include <etaos/error.h>
#include <etaos/mutex.h>

static int eeprom_write(FILE fp, const void *buf, size_t len)
{
	return -EINVAL;
}

static int eeprom_read(FILE fp, void *buf, size_t len)
{
	return -EINVAL;
}

static int eeprom_put(int c, FILE stream)
{
	return -EINVAL;
}

static int eeprom_get(FILE stream)
{
	return -EINVAL;
}

static int eeprom_ioctl(FILE stream, unsigned long reg, void *buf)
{
	return -EINVAL;
}

static struct dev_file_ops eeprom_ops = {
	.write = &eeprom_write,
	.read = &eeprom_read,
	.put = &eeprom_put,
	.get = &eeprom_get,
	.ioctl = &eeprom_ioctl,
};

void eeprom_chip_init(struct eeprom *ee, struct device *dev)
{
	ee->wr_idx = 0;
	ee->rd_idx = 0;
	mutex_init(&ee->lock);

	if(dev)
		device_initialize(dev, &eeprom_ops);
}

static void __used eeprom_init(void)
{
}

subsys_init(eeprom_init);

