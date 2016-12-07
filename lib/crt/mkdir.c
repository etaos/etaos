/*
 *  ETA/OS - LibC mkdir
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
 * @brief Create a new directory.
 * @param path Path to the directory to create.
 * @return An error code.
 * @note This function cannot create directories recursively.
 */
int mkdir(const char *path)
{
	char *base;
	char *dirname;
	struct dirent *dir, *parent;
	unsigned long flags;

	spin_lock_irqsave(&vfs_lock, flags);
	dirname = basename(path);
	base = basepath(path);

	if(!dirname || !base)
		goto err;

	parent = dirent_find(&vfs_root, base);
	if(!parent)
		goto err;

	dir = dirent_create(dirname);
	if(!dir)
		goto err;

	dirent_add_child(parent, dir);

	kfree(dirname);
	kfree(base);
	spin_unlock_irqrestore(&vfs_lock, flags);

	return -EOK;

err:
	if(dirname)
		kfree(dirname);
	if(base)
		kfree(base);

	spin_unlock_irqrestore(&vfs_lock, flags);
	return -EINVAL;
}

/** @} */

