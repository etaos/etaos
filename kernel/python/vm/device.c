/*
 *  ETA/OS - Device ops
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

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/unistd.h>
#include <etaos/python.h>
#include <etaos/vfs.h>
#include <etaos/assert.h>

int device_write(int fd, const void *buffer, size_t num, size_t *addr)
{
	struct file *fp;

	ASSERT(buffer != NULL);
	ASSERT(fd >= 0);

	if(likely(addr)) {
		fp = filep(fd);
		lseek(fp, *addr, SEEK_SET);
	}

	return write(fd, buffer, num) >= 0 ? -EOK : -EINVAL;
}

int device_read(int fd, void *buffer, size_t num, size_t *addr)
{
	struct file *fp;

	ASSERT(buffer != NULL);
	ASSERT(fd >= 0);

	if(likely(addr)) {
		fp = filep(fd);
		lseek(fp, *addr, SEEK_SET);
	}

	return read(fd, buffer, num) >= 0 ? -EOK : -EINVAL;
}
