/*
 *  ETA/OS - ETA/OS STL EEPROM
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
 * @file include/etaos/stl/eeprom.h STL EEPROM header
 */

#ifndef __STL_EEPROM_H__
#define __STL_EEPROM_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

#include <etaos/stl/chardev.h>

/**
 * @addtogroup stl
 * @{
 */

class EEPROM : public CharacterDevice {
public:
	explicit EEPROM(const char *file);
	virtual ~EEPROM(void);

	int read(size_t addr, void *buff, size_t& len);
	int write(size_t addr, const void *buff, size_t& len);
	int putc(size_t addr, int c);

	int operator[](size_t idx);
};

/** @} */

#endif

