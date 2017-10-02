/*
 *  ETA/OS - ATmega flash
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
#include <etaos/device.h>
#include <etaos/flash.h>
#include <etaos/init.h>

#include <asm/io.h>
#include <asm/pgm.h>

#define BLOCK_SIZE 2

static int atmega_flash_read(struct flash_chip *flash, size_t addr, void *buff, size_t len)
{
	ssize_t blocks;
	int remainder;
	size_t indx = 0, ridx;
	uint16_t *block_ptr = buff;
	uint8_t *byte_ptr = buff;

	blocks = len / BLOCK_SIZE;
	remainder = len % BLOCK_SIZE;
	ridx = blocks * BLOCK_SIZE;

	for(; blocks; blocks--, indx++) {
		block_ptr[indx] = pgm_read_word(addr + indx*BLOCK_SIZE);
	}

	for(; remainder; remainder--, ridx++) {
		byte_ptr[ridx] = pgm_read_byte(addr+ridx);
	}

	return -EOK;
}

static int atmega_flash_getc(struct flash_chip *flash, size_t addr)
{
	return pgm_read_byte(addr);
}

static struct flash_chip atmega_flash = {
	.dev = { .name = "flash" },
	.read = &atmega_flash_read,
	.getc = &atmega_flash_getc,
};

static __used void atmega_flash_init(void)
{
	flash_chip_init(&atmega_flash);
}

module_init(atmega_flash_init);
