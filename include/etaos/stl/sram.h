/*
 *  ETA/OS - ETA/OS STL SRAM
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
 * @file include/etaos/stl/sram.h SRAM class header
 */

#ifndef __SRAM_STL_H__
#define __SRAM_STL_H__

/**
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>
#include <etaos/sram.h>

#include <etaos/stl/chardev.h>

/**
 * @brief UAPI class for SRAM chips.
 */
class SRAM : public CharacterDevice {
public:
	explicit SRAM(const char *filename);
	virtual ~SRAM(void);

	int read(size_t addr, void *buff, size_t& len);
	int write(size_t addr, const void *buff, size_t& len);
	int putc(size_t addr, int c);

	int operator[](size_t idx);
};

/** @} */

#endif /* __SRAM_STL_H__ */

