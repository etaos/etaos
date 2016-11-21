/*
 *  ETA/OS - VFS driver.
 *  Copyright (C) 2012, 2014, 2015   Michel Megens
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
 * @file include/etaos/vfs.h VFS header
 */

#ifndef __VFS_H__
#define __VFS_H__

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/list.h>

/**
 * @ingroup vfs
 * @brief Seek operations type.
 */
typedef enum {
	SEEK_SET, //!< Set the index to the input.
	SEEK_CUR, //!< Set the index to the current index + input.
	SEEK_END, //!< Set the index to the file end + input.
} lseek_t;

typedef enum {
	FS_FILE_OPEN,
	FS_FILE_READ,
	FS_FILE_WRITE,
	FS_FILE_CLOSE,

	FS_DIR_CREATE,
	FS_DIR_OPEN,
	FS_DIR_READ,
} fs_ctrl_t;

/**
 * @brief File system driver.
 *
 * Structure describing how a file system operates.
 */
struct fs_driver {
	struct vfile *(*open)(const char *path, int mode); //!< Open a file.
	int (*close)(struct vfile*); //!< File close.
	int (*read)(struct vfile*, void*, size_t); //!< Read from a file.
	int (*write)(struct vfile*, const void*, size_t); //!< Write to a file.
	int (*flush)(struct vfile*); //!< Flush the file.
	int (*put)(int c, struct vfile*); //!< Write 1 byte to a file.
	int (*get)(struct vfile*); //!< Read 1 byte from a file.

	/**
	 * @brief I/O control function pointer.
	 */
	int (*ioctl)(struct vfile*, unsigned long reg, void *buf);
};

struct dirent {
	char *name;

	struct dirent *parent;
	struct list_head entry;
	struct list_head children;

	struct vfile *file_head;
	struct fs_driver *fs;
};

CDECL

extern void vfs_init(void);
extern void vfs_add(struct vfile * file);
extern int vfs_delete(struct vfile * f);
extern struct vfile * vfs_find(const char *name);
extern struct fs_driver *vfs_path_to_fs(const char *path);

extern int vfs_read(struct vfile *file, void *buff, size_t size);
extern int vfs_write(struct vfile *file, const void *buff, size_t size);

extern ssize_t vfs_setoffset(struct vfile *file, ssize_t offset, ssize_t max);
extern size_t lseek(struct vfile *file, size_t offset, int whence);

extern int mkdir(const char *path);
extern int mount(struct fs_driver *fs, const char *path);

/* DIRENT functions */
extern struct dirent *dirent_create(const char *name);
extern struct dirent *dirent_find(struct dirent *root, const char *path);
extern struct dirent *dirent_add_child(struct dirent *parent,
		struct dirent *child);
extern struct vfile *dirent_add_file(struct dirent *dir, struct vfile *file);
extern struct vfile *dirent_remove_file(struct dirent *dir, struct vfile *file);
extern struct vfile *dirent_find_file(struct dirent *dir, const char *filename);

CDECL_END

#endif

