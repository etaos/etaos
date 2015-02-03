/*
 *  ETA/OS - ATmega SPI driver
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
#include <etaos/gpio.h>
#include <etaos/spi.h>
#include <etaos/bitops.h>
#include <etaos/init.h>

#include <asm/io.h>

#define SPCR MEM_IO8(0x4C)
#define SPR0 0x1
#define SPR1 0x2
#define CPHA 0x4
#define CPOL 0x8
#define MSTR 0x10
#define DORD 0x20
#define SPE  0x40
#define SPIE 0x80

#define SPSR MEM_IO8(0x4D)
#define SPI2X 0x1
#define WCOL  0x40
#define SPIF  0x80

#define SPDR MEM_IO8(0x4E)

#define SPI_TMO 500
#define SPI_RETRIES 3

static mutex_t master_spi_mutex;

unsigned char spi_clk_div[] = {
	4,
	16,
	64,
	128,
	2,
	8,
	32,
	64,
};

static int atmega_spi_setspeed(struct spidev *dev, uint32_t rate)
{
	unsigned char div = 0;
	unsigned char spcr;

	for(; div <= 7; div++) {
		if((F_CPU / spi_clk_div[div]) == rate)
			break;
	}

	if(div > 7)
		return -EINVAL;

	if(div > 3) {
		/* enable spi2x */
		set_bit(SPI_2X_FLAG, &dev->flags);
		SPSR |= SPI2X;
	} else {
		SPSR &= 0;
		clear_bit(SPI_2X_FLAG, &dev->flags);
	}
	
	spcr = SPCR;
	spcr &= ~0x3;
	spcr |= div | 0x3;
	SPCR = spcr;

	return -EOK;
}

static int atmega_spi_control(struct spidev *dev, spi_ctrl_t ctrl, void *data)
{
	int rv = -EOK;

	mutex_lock(&master_spi_mutex);
	switch(ctrl) {
	case SPI_MODE0:
		SPCR &= ~(CPHA | CPOL);
		break;

	case SPI_MODE1:
		SPCR |= CPHA;
		break;

	case SPI_MODE2:
		SPCR |= CPOL;
		break;
	
	case SPI_MODE3:
		SPCR |= CPOL | CPHA;
		break;

	case SPI_SET_SPEED:
		rv = atmega_spi_setspeed(dev, *((uint32_t*)data));
		break;

	case SPI_2X:
		SPSR ^= SPI2X;
		break;
	
	default:
		rv = -EINVAL;
		break;
	}
	mutex_unlock(&master_spi_mutex);

	return rv;
}

static int atmega_spi_xfer(struct spidev *dev, struct spi_msg *msg)
{
	int rv = 0;
	size_t idx = 0;
	unsigned char *rx, *tx;

	rx = msg->rx;
	tx = msg->tx;

	mutex_lock(&master_spi_mutex);
	for(; idx < msg->len; idx++) {
		SPDR = tx[idx];
		while(!(SPSR & SPIF));
		rx[idx] = SPDR;
		rv++;
	}
	mutex_unlock(&master_spi_mutex);

	return rv;
}

static struct spi_driver atmega_spi_driver = {
	.name = "atmega-spi",
	.retries = SPI_RETRIES,
	.timeout = SPI_TMO,
	.ctrl = &atmega_spi_control,
	.transfer = &atmega_spi_xfer,
};

static void __used atmega_spi_init(void)
{
	struct gpio_chip *chip = gpio_sys_chip;
	struct gpio_pin *cs;

	/* software initialisation */
	mutex_init(&master_spi_mutex);
	atmega_spi_driver.clk = gpio_chip_to_pin(chip, SCK_GPIO);
	atmega_spi_driver.mosi = gpio_chip_to_pin(chip, MOSI_GPIO);
	atmega_spi_driver.miso = gpio_chip_to_pin(chip, MISO_GPIO);
	spi_bus_init(&atmega_spi_driver);

	/* hardware initialisation */
	cs = gpio_chip_to_pin(chip, SS_GPIO);
	gpio_pin_request(atmega_spi_driver.clk);
	gpio_pin_request(atmega_spi_driver.mosi);
	gpio_pin_request(atmega_spi_driver.miso);
	gpio_pin_request(cs);

	gpio_direction_input(atmega_spi_driver.miso);
	gpio_direction_output(atmega_spi_driver.clk, 0);
	gpio_direction_output(atmega_spi_driver.mosi, 0);
	gpio_direction_output(cs, 1);

	gpio_pin_release(atmega_spi_driver.mosi);
	gpio_pin_release(atmega_spi_driver.miso);
	gpio_pin_release(atmega_spi_driver.clk);
	gpio_pin_release(cs);

	SPCR |= SPR1;
	SPCR |= SPE | MSTR;
	spi_sysbus = &atmega_spi_driver;
}

device_init(atmega_spi_init);

