/*
 *  ETA/OS - LibC write
 *  Copyright (C) 2014   Michel Megens
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
 * @addtogroup libcio
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/bitops.h>
#include <etaos/vfs.h>

/**
 * @brief Write to a file.
 * @param fd File to write to.
 * @param buff Buffer to write into.
 * @param len Size of \p buff.
 */
int write(int fd, const void *buff, size_t len)
{
	struct vfile * file;
	int rv;

	file = __iob[fd];
	if(!file)
		return -EBADF;

	rv = vfs_write(file, buff, len);

	return rv;
}

/** @} */

