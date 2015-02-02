/*
 *  ETA/OS - SPI driver header
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

#ifndef __SPI_DEV_H_
#define __SPI_DEV_H_

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/device.h>
#include <etaos/mutex.h>

struct spidev;

typedef enum spi_ctrl {
	SPI_MODE0, /* CPOL:0 CPHA:0 */
	SPI_MODE1, /* CPOL:0 CPHA:1 */
	SPI_MODE2, /* CPOL:1 CPHA:0 */
	SPI_MODE3, /* CPOL:1 CPHA:1 */
	SPI_SET_SPEED,
	SPI_2X,
} spi_ctrl_t;

struct spi_msg {
	void *rx;
	void *tx;
	size_t len;
};

struct spi_driver {
	const char *name;
	struct list_head devices;
	mutex_t lock;
	char retries;
	int timeout;

	struct gpio_pin *mosi,
			*miso,
			*clk;
	int (*transfer)(struct spidev *spi, struct spi_msg *msg);
	int (*ctrl)(struct spidev *spi, spi_ctrl_t cmd, void *arg);
};

struct spidev {
	struct device dev;
	struct list_head list;
	struct spi_driver *master;
	
	struct gpio_pin *cs;
	unsigned long flags;
#define SPI_CPHA_FLAG      0
#define SPI_CPOL_FLAG      1
#define SPI_CS_HIGH_FLAG   2
#define SPI_2X_FLAG        3

#define SPI_MODE0_MASK 0
#define SPI_MODE1_MASK (0 | (1 << SPI_CPHA_FLAG))
#define SPI_MODE2_MASK ((1 << SPI_CPOL_FLAG) | 0)
#define SPI_MODE3_MASK ((1<<SPI_CPOL_FLAG) | (1<<SPI_CPHA_FLAG))
};

static inline struct spi_msg *spi_alloc_msg(void *rx, void *tx, size_t len)
{
	struct spi_msg *msg;

	if((msg = kzalloc(sizeof(*msg))) == NULL)
		return NULL;

	msg->rx = rx;
	msg->tx = tx;
	msg->len = len;
	return msg;
}

static inline void spi_free_msg(struct spi_msg *msg)
{
	kfree(msg);
}

#endif
