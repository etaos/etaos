/*
 *  ETA/OS - 23K256 SRAM driver
 *  Copyright (C) 2015   Michel Megens <dev@bietje.net>
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
 * @file sram/23k256.c
 */

/*
 * @addtogroup sram-23k256
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdlib.h>
#include <etaos/string.h>
#include <etaos/types.h>
#include <etaos/spi.h>
#include <etaos/gpio.h>
#include <etaos/sram.h>
#include <etaos/mem.h>
#include <etaos/init.h>
#include <etaos/device.h>

#include <etaos/sram/23k256.h>

#define WRSR 0x1
#define RDSR 0x5

#define RDDA 0x3
#define WRDA 0x2

#define HOLD 0x1

#define SRAM_SYNC 5

#define SPI_BYTE_MODE HOLD
#define SPI_PAGE_MODE HOLD | 0x80
#define SPI_SEQ_MODE  HOLD | 0x40

static int __sram_put(struct sram *ram, int c);
static int __sram_get(struct sram *ram);
static int __sram_write(struct sram *ram, const void *_buff, size_t len);
static int __sram_read(struct sram *sram, void *_buff, size_t len);

static struct spidev sram_23k256_dev = {
	.flags = 0UL,
};

static struct sram sram_23k256_chip = {
	.name = "23K256",
	.priv = &sram_23k256_dev,

	.write_byte = &__sram_put,
	.read_byte = &__sram_get,
	.write = &__sram_write,
	.read = &__sram_read,
};

/**
 * @brief Change the mode of the 23K256 chip.
 * @param mode Mode to set.
 */
static void sram_set_mode(unsigned short mode)
{
	unsigned char buff[2];
	struct spi_msg *msg;

	buff[0] = WRSR;
	buff[1] = mode & 0xFF;
	msg = spi_alloc_msg(buff, buff, 2);

	spi_set_speed(&sram_23k256_dev, CONFIG_23K256_SPEED);
	spi_set_mode(&sram_23k256_dev, SPI_MODE0);

	spi_transfer(&sram_23k256_dev, msg);
	spi_free_msg(msg);
}

/**
 * @brief Write a single byte.
 * @param ram SRAM chip descriptor.
 * @param c Character to write.
 * @return Error code or the amount of bytes written.
 */
static int __sram_put(struct sram *ram, int c)
{
	int rv;
	struct spi_msg *msg;
	unsigned char byte = c;
	uint8_t write_seq[] = {
		WRDA, (uint8_t)((ram->file->index >> 8) & 0xFF),
		(uint8_t)(ram->file->index & 0xFF), byte
	};

	msg = spi_alloc_msg(write_seq, write_seq, 4);

	dev_sync_lock(&sram_23k256_dev.dev, SRAM_SYNC);
	sram_set_mode(SPI_BYTE_MODE);
	rv = spi_transfer(&sram_23k256_dev, msg);
	dev_sync_unlock(&sram_23k256_dev.dev);
	spi_free_msg(msg);

	return rv;
}

static int __sram_write(struct sram *ram, const void *_buff, size_t len)
{
	struct spi_msg *msg;
	uint8_t *buff;
	int rv = -EINVAL;

	buff = kzalloc(len+3);
	if(!buff)
		return -ENOMEM;

	memcpy(&buff[3], _buff, len);
	buff[0] = WRDA;
	buff[1] = (uint8_t)((ram->file->index >> 8) & 0xFF);
	buff[2] = (uint8_t)(ram->file->index & 0xFF);
	msg = spi_alloc_msg(buff, buff, len+3);

	dev_sync_lock(&sram_23k256_dev.dev, SRAM_SYNC);
	sram_set_mode(SPI_SEQ_MODE);
	rv = spi_transfer(&sram_23k256_dev, msg);
	dev_sync_unlock(&sram_23k256_dev.dev);
	spi_free_msg(msg);

	kfree(buff);

	return rv;
}

/**
 * @brief Read a single byte from a 23K256 chip.
 * @param ram SRAM chip descriptor.
 * @return The value of the byte read.
 */
static int __sram_get(struct sram *ram)
{
	uint8_t read_seq[] = {
		RDDA, (uint8_t)((ram->file->index >> 8) & 0xFF),
		(uint8_t)(ram->file->index & 0xFF), 0xFF /* dummy byte */
	};
	struct spi_msg *msg = spi_alloc_msg(read_seq, read_seq, 4);

	dev_sync_lock(&sram_23k256_dev.dev, SRAM_SYNC);
	sram_set_mode(SPI_BYTE_MODE);
	spi_transfer(&sram_23k256_dev, msg);
	dev_sync_unlock(&sram_23k256_dev.dev);
	spi_free_msg(msg);

	return read_seq[3];
}

static int __sram_read(struct sram *sram, void *_buff, size_t len)
{
	struct spi_msg *msg;
	uint8_t *buff;
	int rv = -EINVAL;

	buff = kzalloc(len+3);
	if(!buff)
		return -EINVAL;

	buff[0] = RDDA;
	buff[1] = (uint8_t)((sram->file->index >> 8) & 0xFF);
	buff[2] = (uint8_t)(sram->file->index & 0xFF);
	msg = spi_alloc_msg(buff, buff, len+3);

	dev_sync_lock(&sram_23k256_dev.dev, SRAM_SYNC);
	sram_set_mode(SPI_SEQ_MODE);
	rv = spi_transfer(&sram_23k256_dev, msg);
	dev_sync_unlock(&sram_23k256_dev.dev);
	spi_free_msg(msg);

	memcpy(_buff, &buff[3], len);
	kfree(buff);

	return rv;
}

/**
 * @brief Initialise a 23K256 SRAM chip.
 */
void sram_23k256_init(void)
{
	struct gpio_pin *ss = gpio_chip_to_pin(gpio_sys_chip, CONFIG_23K256_SS);

	list_head_init(&sram_23k256_dev.list);

	sram_23k256_dev.cs = ss;
	gpio_pin_request(ss);
	gpio_direction_output(ss, 1);
	gpio_pin_release(ss);

	spi_add_device(spi_sysbus, &sram_23k256_dev);
	sram_chip_init(&sram_23k256_chip, &sram_23k256_dev.dev);
}

chip_init(sram_23k256_init);

/** @} */

