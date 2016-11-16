/*
 *  ETA/OS - Platform implementation
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
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

#include <etaos/stl/chardev.h>

/**
 * @brief Create a new character device.
 * @param file Device filename of the character device.
 */
CharacterDevice::CharacterDevice(const char *file)
{
	this->fd = open(file, _FDEV_SETUP_RW);

	if(this->fd < 0)
		return;

	this->devfile = filep(this->fd);
}

/**
 * @brief Destroy a character device.
 */
CharacterDevice::~CharacterDevice(void)
{
	if(this->fd > 0);
		close(this->fd);
}

/**
 * @brief Check if the character device is opened correctly.
 * @return True if the character device is opened correctly, false otherwise.
 */
bool CharacterDevice::check(void)
{
	if(this->fd < 0)
		return false;

	return true;
}

/**
 * @brief Read from a device file.
 * @param buff Buffer to read data into.
 * @param len Length of \p buff.
 * @return An error code.
 * @see ::read
 */
int CharacterDevice::read(void *buff, size_t& len)
{
	if(!this->check())
		return -EINVAL;

	return ::read(this->fd, buff, len);
}

/**
 * @brief Write data to a character device.
 * @param buff Buffer to write data from.
 * @param len Length of \p buff.
 * @return An error code.
 * @see ::write.
 */
int CharacterDevice::write(const void *buff, size_t& len)
{
	if(!this->check())
		return -EINVAL;

	return ::write(this->fd, buff, len);
}

/**
 * @brief Write a single character to a character device.
 * @param c Character to write.
 * @return An error code.
 * @see ::putc
 */
int CharacterDevice::putc(int& c)
{
	if(!this->check())
		return -EINVAL;

	return ::putc(c, this->devfile);
}

/**
 * @brief Read a single byte from a character device.
 * @return The character read.
 * @see ::getc
 */
int CharacterDevice::getc(void)
{
	if(!this->check())
		return -EINVAL;

	return ::getc(this->devfile);
}

/** @} */

