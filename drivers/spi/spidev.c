/*
 *  ETA/OS - SPI driver
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
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/spi.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/time.h>
#include <etaos/tick.h>

int spi_dev_set_mode(struct spidev *dev, spi_ctrl_t mode)
{
	int rv;
	struct spi_driver *driver;

	if(!dev || mode > SPI_MODE3)
		return -EINVAL;

	driver = dev->master;
	mutex_lock(&driver->lock);

	dev->flags &= ~SPI_MODE0_MASK & 0x3;
	switch(mode) {
	case SPI_MODE1:
		set_bit(SPI_CPHA_FLAG, &dev->flags);
		break;

	case SPI_MODE2:
		set_bit(SPI_CPOL_FLAG, &dev->flags);
		break;

	case SPI_MODE3:
		dev->flags |= 0x3;
		break;

	case SPI_MODE0:
	default:
		break;
	}
	
	rv = driver->ctrl(dev, mode, NULL);
	mutex_unlock(&driver->lock);

	return rv;
}

static int __spi_xfer(struct spidev *dev, struct spi_msg *msg)
{
	int rv;
	char retries;
	tick_t click, ref = sys_tick;
	struct spi_driver *master = dev->master;

	for(retries = 0, rv = 0; retries < master->retries; retries++) {
		rv = master->transfer(dev, msg);

		if(rv != -EAGAIN)
			break;

		click = sys_tick;
		if(time_after(click, ref+master->timeout))
			break;
	}

	return rv;
}

int spi_transfer(struct spidev *dev, struct spi_msg *msg)
{
	int rv;

	mutex_lock(&dev->master->lock);
	rv = __spi_xfer(dev, msg);
	mutex_unlock(&dev->master->lock);

	return rv;
}

int spi_enable_2x(struct spidev *dev)
{
	int ret;
	struct spi_driver *master;

	if(!dev)
		return -EINVAL;
	master = dev->master;

	mutex_lock(&master->lock);
	ret = master->ctrl(dev, SPI_2X, NULL);
	
	if(!ret)
		set_bit(SPI_2X_FLAG, &dev->flags);
	mutex_unlock(&master->lock);

	return ret;
}

int spi_set_speed(struct spidev *dev, uint32_t bps)
{
	int ret;
	struct spi_driver *driver;

	if(!dev || !bps)
		return -EINVAL;

	driver = dev->master;

	mutex_lock(&driver->lock);
	ret = driver->ctrl(dev, SPI_SET_SPEED, &bps);
	mutex_unlock(&driver->lock);

	return ret;
}

int spi_bus_init(struct spi_driver *driver)
{
	if(!driver)
		return -EINVAL;

	mutex_init(&driver->lock);
	list_head_init(&driver->devices);
	return -EOK;
}

void spi_add_device(struct spi_driver *driver, struct spidev *dev)
{
	if(!driver || !dev)
		return;

	list_add(&dev->list, &driver->devices);
	dev->master = driver;
}

static void __used spi_core_init(void)
{
}

subsys_init(spi_core_init);

