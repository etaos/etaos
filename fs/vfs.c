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
static struct file *vfshead;

static spinlock_t vfs_lock;
struct dirent vfs_root = {
	.name = "/",
	.parent = NULL,
	.entry = STATIC_INIT_LIST_HEAD(vfs_root.entry),
	.children = STATIC_INIT_LIST_HEAD(vfs_root.children),
	.file_head = NULL,
	.fs = NULL,
};

int vfs_ioctl(struct dirent *dir, struct file *file, unsigned long reg, void *arg)
{
	struct fs_driver *fs;

	if((fs = dir->fs) == NULL)
		return -EINVAL;

	if(fs->ioctl == NULL)
		return -EINVAL;

	return fs->ioctl(file, reg, arg);
}

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

/**
 * @brief Read from a directory stream.
 * @param dir Directory stream.
 * @param entry Previous entry
 * @param result Pointer to store the result (next entry).
 * @return The next directory entry or NULL.
 */
struct dirent *readdir_r(struct dirent *dir, struct dirent *entry,
		struct dirent **result)
{
	struct dirent *rv, *tmp;
	struct list_head *carriage;
	unsigned long flags;
	bool found = false;

	tmp = NULL;
	rv = NULL;

	spin_lock_irqsave(&vfs_lock, flags);
	list_for_each(carriage, &dir->children) {
		tmp = list_entry(carriage, struct dirent, entry);

		if(found || entry == NULL) {
			rv = tmp;
			break;
		}

		if(tmp == entry)
			found = true;
	}
	spin_unlock_irqrestore(&vfs_lock, flags);

	*result = rv;
	return rv;
}

/**
 * @brief Read from a directory stream.
 * @param dirp Directory stream.
 * @return The next directory entry.
 * @note This function is not thread safe.
 * @see readdir_r
 */
struct dirent *readdir(struct dirent *dirp)
{
	static struct dirent *entry = NULL;
	static struct dirent *result = NULL;

	entry = readdir_r(dirp, entry, &result);
	return result;
}

/**
 * @brief Close a directory stream.
 * @param dirp Directory stream to close.
 * @return An error code.
 */
int closedir(struct dirent *dirp)
{
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
	else
		vfs_ioctl(dir, file, FS_FILE_UNLINK, NULL);
err_l:
	spin_unlock_irqrestore(&vfs_lock, flags);
	kfree(filepath);
	kfree(filename);

	return err;
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

	vfshead = NULL;
}

/* @} */

