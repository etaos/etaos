/*
 *  ETA/OS - Python I2C library
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

#undef __FILE_ID__
#define __FILE_ID__ 0x1B

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/python.h>
#include <etaos/eeprom.h>
#include <etaos/vfs.h>
#include <etaos/unistd.h>

int pm_ee_read(uint8_t addr, void *buff, size_t len)
{
	int rc, fd;
	struct file *stream;

	fd = open("/dev/24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		lseek(stream, addr, SEEK_SET);
		rc = read(fd, buff, len);
		close(fd);
	} else {
		rc = -1;
	}

	return rc;
}

int pm_ee_write(uint8_t addr, const void *buff, size_t len)
{
	int fd, rc = -EOK;
	struct file *stream;

	fd = open("/dev/24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		lseek(stream, addr, SEEK_SET);
		write(fd, buff, len);
		close(fd);
	} else {
		rc = -1;
	}

	return rc;
}

