/*
 *  ETA/OS - LibC lseek
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
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/unistd.h>

/**
 * @brief The lseek function repositions the offset of the given file.
 * @param file Index of \p file will be repositioned.
 * @param offset Number bytes to offset \p file 's index.
 * @param whence Way to offset \p file 's index.
 * @return The resulting offset location (file index) from the start of the 
 *         file.
 *
 * The index of \p file will be repositioned arrording to \p whence as follows:
 *
 * \b SEEK_SET \n
 * The index is set to \p offset.
 *
 * \b SEEK_CUR \n
 * The index is set to its current location plus \p offset bytes.
 *
 * \b SEEK_END \n
 * The offset is set to the size of the file plus \p offset bytes.
 */
size_t lseek(struct file *file, size_t offset, int whence)
{
	if(file->lseek)
		return file->lseek(file, offset, whence);

	switch(whence) {
	case SEEK_END:
		offset += file->length;
		break;

	case SEEK_CUR:
		if(!offset)
			return file->index;

		return vfs_setindex(file, file->index + offset, 0);
	default:
		break;
	}

	return vfs_setindex(file, offset, 0);
}

/** @} */

