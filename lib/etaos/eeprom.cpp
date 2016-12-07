/*
 *  ETA/OS - EEPROM implementation
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/unistd.h>
#include <etaos/vfs.h>

#include <etaos/stl/chardev.h>
#include <etaos/stl/eeprom.h>

EEPROM::EEPROM(const char *file) : CharacterDevice(file)
{
}

EEPROM::~EEPROM(void)
{
}

/**
 * @brief Read from an EEPROM chip.
 * @param addr Address to start reading.
 * @param buff Buffer to read into.
 * @param len Length of \p buff.
 * @return An error code.
 * @see ::read
 */
int EEPROM::read(size_t addr, void *buff, size_t& len)
{
	lseek(this->devfile, addr, SEEK_SET);
	return CharacterDevice::read(buff, len);
}

/**
 * @brief Write to an EEPROM chip.
 * @param addr Address to start writing at.
 * @param buff Buffer to write from.
 * @param len Length of \p buff.
 * @return An error code.
 * @see ::write
 */
int EEPROM::write(size_t addr, const void *buff, size_t& len)
{
	lseek(this->devfile, addr, SEEK_SET);
	return CharacterDevice::write(buff, len);
}

/**
 * @brief Write a single character.
 * @param addr Address to write to.
 * @param c Character to write.
 * @return An error code.
 * @see ::putc
 */
int EEPROM::putc(size_t addr, int c)
{
	lseek(this->devfile, addr, SEEK_SET);
	return CharacterDevice::putc(c);
}

/**
 * @brief Read a single character.
 * @param idx Address to read from.
 * @return The read character.
 * @see CharacterDevice::getc
 */
int EEPROM::operator[](size_t idx)
{
	lseek(this->devfile, idx, SEEK_SET);
	return CharacterDevice::getc();
}

