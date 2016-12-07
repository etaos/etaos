/*
 *  ETA/OS - VFS driver.
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
#include <etaos/string.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>
#include <etaos/init.h>
#include <etaos/mem.h>
#include <etaos/spinlock.h>

#include <etaos/fs/util.h>
#include <etaos/fs/basename.h>

struct file * __iob[MAX_OPEN];

spinlock_t vfs_lock;
struct dirent vfs_root = {
	.name = "/",
	.parent = NULL,
	.entry = STATIC_INIT_LIST_HEAD(vfs_root.entry),
	.children = STATIC_INIT_LIST_HEAD(vfs_root.children),
	.file_head = NULL,
	.fs = NULL,
};

/**
 * @brief Add a new file to the VFS.
 * @param path Path to add \p file to.
 * @param file File to add.
 * @return An error code.
 */
int vfs_add_file(const char *path, struct file *file)
{
	struct dirent *dir;
	unsigned long flags;

	spin_lock_irqsave(&vfs_lock, flags);

	dir = dirent_find(&vfs_root, path);
	if(!dir) {
		spin_unlock_irqrestore(&vfs_lock, flags);
		return -EINVAL;
	}

	dirent_add_file(dir, file);
	spin_unlock_irqrestore(&vfs_lock, flags);

	return -EOK;
}

/**
 * @brief Get the file system driver of a path.
 * @param path Path to get the file system driver for.
 * @return The file system driver behind \p path.
 */
struct fs_driver *vfs_path_to_fs(const char *path)
{
	struct dirent *dir;
	unsigned long flags;

	spin_lock_irqsave(&vfs_lock, flags);
	dir = dirent_find(&vfs_root, path);
	spin_unlock_irqrestore(&vfs_lock, flags);


	return dir ? dir->fs : NULL;
}

/**
 * @brief Find a file in the VFS.
 * @param path Path to the file to find.
 * @return The found file.
 * @retval NULL if no file was found.
 */
struct file *vfs_find_file(const char *path)
{
	unsigned long flags;
	struct dirent *dir;
	char *filepath, *filename;
	struct file *file;

	filepath = basepath(path);
	filename = basename(path);

	if(!filepath || !filename) {
		if(filename)
			kfree(filename);
		if(filepath)
			kfree(filepath);

		return NULL;
	}

	spin_lock_irqsave(&vfs_lock, flags);
	dir = dirent_find(&vfs_root, filepath);
	/* dirent_find_file handles NULL dirs correctly */
	file = dirent_find_file(dir, filename);
	spin_unlock_irqrestore(&vfs_lock, flags);

	kfree(filepath);
	kfree(filename);
	return file;
}

/**
 * @brief Add a file to the file descriptor array.
 * @param stream File to add.
 * @return Assigned file descriptor.
 * @retval -1 on error.
 */
int iob_add(struct file * stream)
{
	int rc;

	for(rc = 3; rc < MAX_OPEN; rc++) {
		if(__iob[rc] == NULL) {
			__iob[rc] = stream;
			stream->fd = rc;
			return rc;
		}
	}
	return -1;
}

/**
 * @brief Remove a file from the FD array.
 * @param fd File descriptor to remove.
 * @retval -EOK on success.
 * @retval -EINVAL on error.
 */
int iob_remove(int fd)
{
	if(__iob[fd]) {
		__iob[fd]->fd = 0;
		__iob[fd] = NULL;
		return -EOK;
	} else {
		return -EINVAL;
	}
}

/**
 * @brief Initialise the VFS.
 */
void vfs_init(void)
{
	int i = 3;

	spinlock_init(&vfs_lock);
	for(; i < MAX_OPEN; i++)
		__iob[i] = NULL;
}

/* @} */

