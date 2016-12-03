/*
 *  ETA/OS - ETA/OS STL character device header
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
 * @file include/etaos/stl/chardev.h Character device header
 */

#ifndef __CHARDEV__
#define __CHARDEV__

/**
 * @addtogroup stl
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

/**
 * @brief Representation of a character device.
 */
class CharacterDevice {
public:
	explicit CharacterDevice(const char *file);
	virtual ~CharacterDevice();

	int read(void *buff, size_t& len);
	int write(const void *buff, size_t& len);
	int getc();
	int putc(int& c);

protected:
	struct file *devfile; //!< Backend device file.
	int fd; //!< Backend file descriptor.

private:
	bool check(void);
};

/** @} */

#endif

