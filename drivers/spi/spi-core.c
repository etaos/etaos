/*
 *  ETA/OS - SPI driver
 *  Copyright (C) 2015   Michel Megens <dev@bietje.net>
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

/**
 * @file spi/spidev.c
 * @addtogroup spi
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/spi.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/gpio.h>

/**
 * @brief SPI system bus
 */
struct spi_driver *spi_sysbus;

/**
 * @brief Change the SPI bus mode.
 * @param dev Device requesting the mode change.
 * @param mode New mode to set.
 * @return Error code.
 */
int spi_set_mode(struct spidev *dev, spi_ctrl_t mode)
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

/**
 * @brief SPI core backend for SPI transmissions.
 * @param dev Device in control of the transmission.
 * @param msg Message which is to be transferred.
 * @return Error code or the amount of bytes transferred.
 */
static int __spi_xfer(struct spidev *dev, struct spi_msg *msg)
{
	int rv;
	char retries;
	tick_t click, ref = sys_tick;
	struct spi_driver *master = dev->master;

	gpio_pin_request(dev->cs);
	if(test_bit(SPI_CS_HIGH_FLAG, &dev->flags))
		gpio_pin_write(dev->cs, 1);
	else
		gpio_pin_write(dev->cs, 0);

	for(retries = 0, rv = 0; retries < master->retries; retries++) {
		rv = master->transfer(dev, msg);

		if(rv != -EAGAIN)
			break;

		click = sys_tick;
		if(time_after(click, ref+master->timeout))
			break;
	}

	if(test_bit(SPI_CS_HIGH_FLAG, &dev->flags))
		gpio_pin_write(dev->cs, 0);
	else
		gpio_pin_write(dev->cs, 1);

	gpio_pin_release(dev->cs);

	return rv;
}

/**
 * @brief Transfer an SPI message.
 * @param dev Device requesting the transmission.
 * @param msg Message to transfer.
 * @return Amount of bytes transferred.
 */
int spi_transfer(struct spidev *dev, struct spi_msg *msg)
{
	int rv;

	mutex_lock(&dev->master->lock);
	rv = __spi_xfer(dev, msg);
	mutex_unlock(&dev->master->lock);

	return rv;
}

/**
 * @brief Enable the SPI 2X mode.
 * @param dev Device requesting the mode.
 * @return An error code.
 * @note This will double the bitrate of the SPI bus.
 */
int spi_enable_2x(struct spidev *dev)
{
	int ret;
	struct spi_driver *master;

	if(!dev)
		return -EINVAL;
	master = dev->master;

	mutex_lock(&master->lock);
	ret = master->ctrl(dev, SPI_2X, NULL);
	
	if(!ret) {
		if(test_bit(SPI_2X_FLAG, &dev->flags))
			clear_bit(SPI_2X_FLAG, &dev->flags);
		else
			set_bit(SPI_2X_FLAG, &dev->flags);
	}
	mutex_unlock(&master->lock);

	return ret;
}

/**
 * @brief Set the bitrate of the SPI bus.
 * @param dev Device requesting the bitrate.
 * @param bps BPS in hertz.
 * @return Error code.
 */
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

/**
 * @brief Initialise a new SPI bus.
 * @param driver Bus to initialise.
 * @return An error code.
 */
int spi_bus_init(struct spi_driver *driver)
{
	if(!driver)
		return -EINVAL;

	list_head_init(&driver->devices);
	return -EOK;
}

/**
 * @brief Add a new SPI device to an existing SPI bus.
 * @param driver SPI bus to add \p dev to.
 * @param dev SPI device which needs a master bus.
 */
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

/**
 * @}
 */

subsys_init(spi_core_init);

