/*
 *  ETA/OS - SRAM core header
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
 * @file etaos/sram.h
 */
/**
 * @addtogroup sram
 * @{
 */

#ifndef __SRAM_H_
#define __SRAM_H_

#include <etaos/kernel.h>
#include <etaos/device.h>
#include <etaos/mutex.h>

/**
 * @brief SRAM descriptor.
 */
struct sram {
	const char *name; //!< Chip name.
	struct vfile *file; //!< SRAM device file.
	
	void *priv; //!< Private data.

	/**
	 * @brief Read function pointer.
	 * @param ram SRAM chip descriptor.
	 * @param buf Buffer to read data into.
	 * @param len Length of \p buf.
	 * @return Error code or number bytes read.
	 */
	int (*read)(struct sram *ram, void *buf, size_t len);
	
	/**
	 * @brief Write function pointer.
	 * @param ram SRAM chip descriptor.
	 * @param buf Buffer which has to be written.
	 * @param len Length of \p buf.
	 * @return Error code or the number of bytes written.
	 */
	int (*write)(struct sram *ram, const void *buf, size_t len);

	/**
	 * @brief Read a single byte from the SRAM chip memory.
	 * @param ram SRAM chip descriptor.
	 * @return Error code or the number of bytes read.
	 */
	int (*read_byte)(struct sram *ram);
	
	/**
	 * @brief Write a single byte to the SRAM chip memory.
	 * @param ram SRAM chip descriptor.
	 * @param c Byte which has to be written.
	 * @return Error code or the number of bytes written.
	 */
	int (*write_byte)(struct sram *ram, int c);
};

CDECL
extern void sram_chip_init(struct sram *ram, struct device *dev);
CDECL_END

#endif

/** @} */

