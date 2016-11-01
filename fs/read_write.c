/*
 *  ETA/OS - VFS read/write operations
 *  Copyright (C) 2012   Michel Megens
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
 * @addtogroup vfs
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/bitops.h>

/**
 * @brief Write to a file.
 * @param file File to write to.
 * @param buff Buffer to write into.
 * @param size Size of \p buff.
 * @return The number of bytes written.
 */
int vfs_write(struct vfile *file, const void *buff, size_t size)
{
	if(test_bit(STREAM_WRITE_FLAG, &file->flags) && file->write)
		return file->write(file, buff, size);
	else
		return -EINVAL;
}

/**
 * @brief Read from a file.
 * @param file File to read from.
 * @param buff Buffer to store data into.
 * @param size Size of \p buff.
 * @return The number of bytes read.
 */
int vfs_read(struct vfile *file, void *buff, size_t size)
{
	if(test_bit(STREAM_READ_FLAG, &file->flags) && file->read)
		return file->read(file, buff, size);
	else
		return -EINVAL;
}

/** @} */

