/*
 *  ETA/OS - EEPROM header
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdlib.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>
#include <etaos/eeprom.h>
#include <etaos/init.h>
#include <etaos/device.h>

#include <asm/io.h>
#include <asm/eeprom.h>

static inline int eeprom_can_write_to(uint16_t addr)
{
	return addr < EEPROM_SIZE;
}

static int atmega_eeprom_write(struct eeprom *e, const void *buf, size_t len)
{
	struct file *stream;
	uint16_t addr;

	stream = e->file;
	addr = (uint16_t)stream->index;

	if(unlikely(!eeprom_can_write_to(addr)))
		return -EOF;

	eeprom_write_block(addr, buf, len);
	stream->index += len;

	return -EOK;
}

static int atmega_eeprom_read(struct eeprom *e, void *buf, size_t len)
{
	struct file *stream;
	uint16_t addr;

	stream = e->file;
	addr = (uint16_t)stream->index;

	if(unlikely(!eeprom_can_write_to(addr)))
		return -EOF;

	eeprom_read_block(addr, buf, len);
	stream->index += len;

	return -EOK;
}

static int atmega_eeprom_read_byte(struct eeprom *e)
{
	uint8_t var;
	struct file *stream;
	uint16_t addr;

	stream = e->file;
	addr = (uint16_t)stream->index;

	if(unlikely(!eeprom_can_write_to(addr)))
		return -EOF;

	eeprom_read_block(addr, &var, 1);
	stream->index ++;

	return (int)var;
}

static int atmega_eeprom_write_byte(struct eeprom *e, int c)
{
	struct file *stream;
	uint16_t addr;

	stream = e->file;
	addr = (uint16_t)stream->index;

	if(unlikely(!eeprom_can_write_to(addr)))
		return -EOF;

	eeprom_write_block(addr, (const void *)&c, 1);
	stream->index ++;
	return -EOK;
}

static struct eeprom atmega_eeprom = {
	.name = "atmega-eeprom",
	.write = &atmega_eeprom_write,
	.read = &atmega_eeprom_read,
	.read_byte = &atmega_eeprom_read_byte,
	.write_byte = &atmega_eeprom_write_byte,
};

static struct device atmega_ee_device;

static __used void atmega_eeprom_init(void)
{
	eeprom_chip_init(&atmega_eeprom, &atmega_ee_device);
}

device_init(atmega_eeprom_init);
