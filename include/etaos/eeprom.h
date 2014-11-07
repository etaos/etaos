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

/**
 * @file etaos/eeprom.h
 */

#ifndef __EEPROM__
#define __EEPROM__

#include <etaos/kernel.h>
#include <etaos/device.h>
#include <etaos/mutex.h>

/**
 * @brief EEPROM chip descriptor.
 */
struct eeprom {
	const char *name;

	unsigned long rd_idx; //!< Chip address read index.
	unsigned long wr_idx; //!< Chip address write index.

	mutex_t lock; //!< Chip lock.
	void *priv; //!< Private data, usually set to the i2c_client.

	int (*read)(struct eeprom *ee, void *buf, size_t len);
	int (*write)(struct eeprom *ee, const void *buf, size_t len);
	int (*read_byte)(struct eeprom *ee);
	int (*write_byte)(struct eeprom *ee, int c);
};

typedef enum {
	EEPROM_RESET_WR_IDX,
	EEPROM_RESET_RD_IDX,
} eeprom_ioctl_t;

CDECL
extern void eeprom_chip_init(struct eeprom *ee, struct device *dev);
CDECL_END

#endif
