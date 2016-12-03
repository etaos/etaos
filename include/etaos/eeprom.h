/*
 *  ETA/OS - EEPROM header
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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
 * @file etaos/eeprom.h
 */

/**
 * @addtogroup ee
 * @{
 */

#ifndef __EEPROM__
#define __EEPROM__

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/device.h>
#include <etaos/mutex.h>

/**
 * @brief EEPROM chip descriptor.
 */
struct eeprom {
	const char *name; //!< EEPROM chip name.
	struct file *file; //!< EEPROM device file.

	void *priv; //!< Private data, usually set to the i2c_client.

	/**
	 * @brief Read function pointer.
	 * @param ee EEPROM chip descriptor.
	 * @param buf Buffer to read data into.
	 * @param len Length of \p buf.
	 * @return Error code.
	 */
	int (*read)(struct eeprom *ee, void *buf, size_t len);

	/**
	 * @brief Write function pointer.
	 * @param ee EEPROM chip descriptor.
	 * @param buf Buffer which has to be written.
	 * @param len Length of \p buf.
	 * @return Error code.
	 */
	int (*write)(struct eeprom *ee, const void *buf, size_t len);

	/**
	 * @brief Read a single byte from the EEPROM chip memory.
	 * @param ee EEPROM chip descriptor.
	 * @return Error code.
	 */
	int (*read_byte)(struct eeprom *ee);

	/**
	 * @brief Write a single byte to the EEPROM chip memory.
	 * @param ee EEPROM chip descriptor.
	 * @param c Byte which has to be written.
	 * @return Error code.
	 */
	int (*write_byte)(struct eeprom *ee, int c);
};

CDECL
extern void eeprom_chip_init(struct eeprom *ee, struct device *dev);
CDECL_END

#endif

/** @} */

