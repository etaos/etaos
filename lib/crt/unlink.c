/*
 *  ETA/OS - LibC unlink
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
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/unistd.h>
#include <etaos/vfs.h>
#include <etaos/mem.h>

#include <etaos/fs/basename.h>

/**
 * @brief Remove a file from the VFS.
 * @param path Path to the file to remove.
 * @return An error code.
 */
int unlink(const char *path)
{
	unsigned long flags;
	struct dirent *dir;
	char *filepath, *filename;
	struct file *file;
	int err = -EOK;

	filepath = basepath(path);
	filename = basename(path);

	if(!filepath || !filename) {
		if(filename)
			kfree(filename);
		if(filepath)
			kfree(filepath);

		return -EINVAL;
	}

	spin_lock_irqsave(&vfs_lock, flags);
	dir = dirent_find(&vfs_root, filepath);
	/* dirent_find_file handles NULL dirs correctly */
	file = dirent_find_file(dir, filename);

	if(!file) {
		err = -EINVAL;
		goto err_l;
	}

	if(dirent_remove_file(dir, file) != file)
		err = -EINVAL;

err_l:
	spin_unlock_irqrestore(&vfs_lock, flags);
	kfree(filepath);
	kfree(filename);

	return err;
}

/** @} */

