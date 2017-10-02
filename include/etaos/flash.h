/*
 *  ETA/OS - FLASH header
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

#ifndef __FLASH_H__
#define __FLASH_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/device.h>

struct flash_chip {
	struct device dev;

	int (*read)(struct flash_chip *flash, size_t addr, void *buff, size_t length);
	int  (*getc)(struct flash_chip *flash, size_t addr);
};

CDECL
extern void flash_chip_init(struct flash_chip *flash);
CDECL_END

#endif /* end of include guard: __FLASH_H__ */
