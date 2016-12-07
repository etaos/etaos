/*
 *  ETA/OS - Generic file operations
 *  Copyright (C) 2015   Michel Megens
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
#include <etaos/vfs.h>

/**
 * @brief Set the current index of a file.
 * @param file File to set the index off.
 * @param offset Signed offset to set.
 * @param max Maximum offset allowed to set.
 * @return The offset of the file from the beginning of the file.
 *
 * Note that this function is equal to:
@code{.c}
	lseek(file, SEEK_SET, offset);
@endcode
  */
ssize_t vfs_setoffset(struct file *file, ssize_t offset, ssize_t max)
{
	if(offset > max)
		return -EINVAL;
	if(offset < 0 && (file->index + offset) < 0)
		return -EINVAL;

	if(offset != file->index)
		file->index += offset;

	return offset;
}

/**
 * @brief Set the index of a file.
 * @param file File to set the index for.
 * @param index Index to set.
 * @param max Maximum index.
 * @return The index that was set for \p file.
 */
size_t vfs_setindex(struct file *file, size_t index, size_t max)
{
	if(index > max && max)
		return -EINVAL;

	if(index != file->index)
		file->index  = index;

	return index;
}

/** @} */

