/*
 *  ETA/OS - SRAM core driver
 *  Copyright (C) 2015   Michel Megens <dev@michelmegens.net>
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
#include <etaos/sram.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/device.h>
#include <etaos/error.h>
#include <etaos/mutex.h>

static inline struct sram *to_sram_chip(FILE file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	return dev->dev_data;
}

static int sram_write(FILE stream, const void *buf, size_t len)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	mutex_lock(&ram->lock);
	rc = ram->write(ram, buf, len);
	ram->wr_idx += len;
	mutex_unlock(&ram->lock);

	return rc;
}

static int sram_read(FILE stream, void *buf, size_t len)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	mutex_lock(&ram->lock);
	rc = ram->read(ram, buf, len);
	ram->rd_idx += len;
	mutex_unlock(&ram->lock);

	return rc;
}

static int sram_put(int c, FILE stream)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	mutex_lock(&ram->lock);
	rc = ram->write_byte(ram, c);
	ram->wr_idx++;
	mutex_unlock(&ram->lock);

	return rc;
}

static int sram_get(FILE stream)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	mutex_lock(&ram->lock);
	rc = ram->read_byte(ram);
	ram->rd_idx++;
	mutex_unlock(&ram->lock);

	return rc;
}

static int sram_ioctl(FILE stream, unsigned long reg, void *buf)
{
	int rc;
	unsigned long idx;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);

	if(!buf)
		idx = 0;
	else
		idx = *((unsigned long*)buf);

	mutex_lock(&ram->lock);
	switch(reg) {
	case SRAM_RESET_WR_IDX:
		ram->wr_idx = idx;
		rc = -EOK;
		break;

	case SRAM_RESET_RD_IDX:
		ram->rd_idx = idx;
		rc = -EOK;
		break;

	default:
		rc = -EINVAL;
		break;
	}
	mutex_unlock(&ram->lock);

	return rc;
}

static struct dev_file_ops sram_ops = {
	.write = &sram_write,
	.read = &sram_read,
	.put = &sram_put,
	.get = &sram_get,
	.ioctl = &sram_ioctl,
};

void sram_chip_init(struct sram *ram, struct device *dev)
{
	ram->rd_idx = 0;
	ram->wr_idx = 0;

	mutex_init(&ram->lock);
	if(dev) {
		dev->name = ram->name;
		device_initialize(dev, &sram_ops);
		dev->dev_data = ram;
	}
}

static void __used sram_init(void)
{
}

subsys_init(sram_init);

