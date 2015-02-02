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

#ifndef __SRAM_H_
#define __SRAM_H_

#include <etaos/kernel.h>
#include <etaos/device.h>
#include <etaos/mutex.h>

struct sram {
	const char *name;
	
	unsigned long rd_idx,
		      wr_idx;
	mutex_t lock;
	void *priv;

	int (*read)(struct sram *ram, void *buf, size_t len);
	int (*write)(struct sram *ram, const void *buf, size_t len);

	int (*read_byte)(struct sram *ram);
	int (*write_byte)(struct sram *ram, int c);
};

typedef enum {
	SRAM_RESET_WR_IDX,
	SRAM_RESET_RD_IDX,
} sram_ioctl_t;

CDECL
extern void sram_chip_init(struct sram *ram, struct device *dev);
CDECL_END

#endif
