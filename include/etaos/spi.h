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

/**
 * @file etaos/spi.h
 * @addtogroup spi
 * @{
 */

#ifndef __SPI_DEV_H_
#define __SPI_DEV_H_

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/device.h>

struct spidev;

/**
 * @brief SPI control options (commands).
 */
typedef enum spi_ctrl {
	SPI_MODE0, //!< CPOL:0 CPHA:0
	SPI_MODE1, //!< CPOL:0 CPHA:1
	SPI_MODE2, //!< CPOL:1 CPHA:0
	SPI_MODE3, //!< CPOL:1 CPHA:1
	SPI_SET_SPEED, //!< Set the SPI bitrate.
	SPI_2X, //!< Enable SPI double bitrate (if supported).
} spi_ctrl_t;

/**
 * @brief SPI transmission message structure.
 * @note spi_msg::rx and spi_msg::tx can point to the same buffer. In this
 *       case data will be over written after the write operation.
 */
struct spi_msg {
	void *rx; //!< Receive buffer.
	void *tx; //!< Transmission buffer.
	size_t len; //!< Length of rx and tx.
};

/**
 * @brief SPI driver structure. Represents an SPI bus.
 * @note Implemented by SPI bus drivers
 */
struct spi_driver {
	const char *name; //!< Name of the bus.
	struct list_head devices; //!< Amount of attached SPI devices.
	char retries; //!< Transmission retry number.
	int timeout; //!< Transmission timeout.

	struct gpio_pin *mosi, //!< Master Output Slave Input
			*miso, //!< Master Input Slave Output
			*clk; //!< Serial clock.
	/**
	 * @brief Start a new transmission.
	 * @param spi SPI device driver starting the transmission.
	 * @param msg Message to be transferred.
	 * @return Amount of bytes transferred.
	 */
	int (*transfer)(struct spidev *spi, struct spi_msg *msg);
	/**
	 * @brief Control the SPI bus.
	 * @param spi Device changing control options.
	 * @param cmd Control command.
	 * @param arg Argument to \p cmd.
	 */
	int (*ctrl)(struct spidev *spi, spi_ctrl_t cmd, void *arg);
};

/**
 * @brief SPI device structure.
 * @brief Implemented by SPI device drivers (i.e. an SRAM chip).
 */
struct spidev {
	struct device dev; //!< Underlying device.
	struct list_head list; //!< List entry.
	struct spi_driver *master; //!< Bus master.
	
	struct gpio_pin *cs; //!< Chip select pin.
	unsigned long flags; //!< Flags.
/**
 * @name SPI flags
 * @{
 */
#define SPI_CPHA_FLAG      0 //!< Clock phase selector.
#define SPI_CPOL_FLAG      1 //!< Clock polarity selector.
#define SPI_CS_HIGH_FLAG   2 //!< CS active on HIGH.
#define SPI_2X_FLAG        3 //!< SPI double speed flag.
/**
 * @}
 */

#define SPI_MODE0_MASK 0
#define SPI_MODE1_MASK (0 | (1 << SPI_CPHA_FLAG))
#define SPI_MODE2_MASK ((1 << SPI_CPOL_FLAG) | 0)
#define SPI_MODE3_MASK ((1<<SPI_CPOL_FLAG) | (1<<SPI_CPHA_FLAG))
};

/**
 * @brief Allocate a new SPI message.
 * @param rx Receive buffer.
 * @param tx Transmit buffer.
 * @param len Length of \p rx and \p tx.
 * @return The allocated SPI message.
 */
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

/**
 * @brief Free a SPI message.
 * @param msg Message which has to be free'd.
 * @note Only free message's allocated with spi_alloc_msg.
 */
static inline void spi_free_msg(struct spi_msg *msg)
{
	kfree(msg);
}

CDECL
extern struct spi_driver *spi_sysbus;
extern int spi_bus_init(struct spi_driver *driver);
extern void spi_add_device(struct spi_driver *driver, struct spidev *dev);
extern int spi_set_speed(struct spidev *dev, uint32_t bps);
extern int spi_enable_2x(struct spidev *dev);
extern int spi_transfer(struct spidev *dev, struct spi_msg *msg);
extern int spi_set_mode(struct spidev *dev, spi_ctrl_t mode);
CDECL_END

#endif

/** @} */

