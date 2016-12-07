/*
 *  ETA/OS - LibC opendir
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
#include <etaos/dirent.h>
#include <etaos/vfs.h>
#include <etaos/error.h>

/**
 * @brief Open a directory stream.
 * @param dirname Directory to open.
 * @return A directory stream or NULL.
 */
struct dirent *opendir(const char *dirname)
{
	struct dirent *dir;
	unsigned long flags;

	spin_lock_irqsave(&vfs_lock, flags);
	dir = dirent_find(&vfs_root, dirname);
	spin_unlock_irqrestore(&vfs_lock, flags);

	return dir;
}

/** @} */

