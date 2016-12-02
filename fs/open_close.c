/*
 *  ETA/OS - VFS driver.
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
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/vfs.h>
#include <etaos/mem.h>

#include <etaos/fs/util.h>
#include <etaos/fs/basename.h>

int vfs_open(const char *path, int mode)
{
	struct vfile *file;
	int err = -EINVAL;
	char *dir;
	struct fs_driver *fs;

	file = vfs_find_file(path);
	if(file) {
		file->flags = mode;
		if(file->open)
			err = file->open(file);
		else
			err = -EOK;

		if(!err) {
			iob_add(file);
			err = file->fd;
		}
	} else {
		dir = basepath(path);
		err = -EINVAL;

		fs = vfs_path_to_fs(dir);

		if(fs && fs->open) {
			file = fs->open(path, mode);
			if(file) {
				file->flags = mode;
				if(file->open)
					file->open(file);
				iob_add(file);
				err = file->fd;
			}
		}

		kfree(dir);
	}

	return err;
}

int vfs_close(int fd)
{
	struct vfile *file;

	file = __iob[fd];
	if(file) {
		if(file->close)
			file->close(file);

		iob_remove(fd);
		file->flags = 0UL;
	}

	return -EOK;
}

