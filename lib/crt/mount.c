/*
 *  ETA/OS - LibC mount
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
#include <etaos/dirent.h>
#include <etaos/mem.h>

#include <etaos/fs/basename.h>

/**
 * @brief Mount a file system to a path.
 * @param fs File system to mount.
 * @param path Path to mount \p fs on.
 * @return An error code.
 * @retval -EOK on success.
 * @retval -EINVAL on failure.
 */
int mount(struct fs_driver *fs, const char *path)
{
	struct dirent *dir;
	unsigned long flags;

	if(!fs || !path)
		return -EINVAL;

	spin_lock_irqsave(&vfs_lock, flags);
	if((dir = dirent_find(&vfs_root, path)) == NULL) {
		spin_unlock_irqrestore(&vfs_lock, flags);
		return -EINVAL;
	}

	dir->fs = fs;
	spin_unlock_irqrestore(&vfs_lock, flags);
	return -EOK;
}

/** @} */

