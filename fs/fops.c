/*
 *  ETA/OS - Generic file operations
 *  Copyright (C) 2012   Michel Megens
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

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

ssize_t vfs_setoffset(struct vfile *file, ssize_t offset, ssize_t max)
{
	if(offset > max)
		return -EINVAL;
	if(offset < 0 && (file->index + offset) < 0)
		return -EINVAL;

	if(offset != file->index)
		file->index += offset;

	return offset;
}

static size_t vfs_setindex(struct vfile *file, size_t index, size_t max)
{
	if(index > max && max)
		return -EINVAL;
	
	if(index != file->index)
		file->index  = index;

	return index;
}

size_t lseek(struct vfile *file, size_t offset, int whence)
{
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

