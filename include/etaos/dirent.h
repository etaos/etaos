/*
 *  ETA/OS - Dirent header
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

#ifndef __DIRENT_H__
#define __DIRENT_H__

#include <etaos/kernel.h>
#include <etaos/stdio.h>

/**
 * @brief Directory entry descriptor.
 */
struct dirent {
	char *name; //!< Directory name.

	struct dirent *parent; //!< Parent directory.
	struct list_head entry; //!< List entry.
	struct list_head children; //!< List head of child directory's.

	struct file *file_head; //!< List of file entry's.
	struct fs_driver *fs;    //!< File system driver.
};

CDECL
/* DIRENT functions */
extern struct dirent *dirent_create(const char *name);
extern struct dirent *dirent_find(struct dirent *root, const char *path);
extern struct dirent *dirent_add_child(struct dirent *parent,
		struct dirent *child);
extern struct file *dirent_add_file(struct dirent *dir, struct file *file);
extern struct file *dirent_remove_file(struct dirent *dir, struct file *file);
extern struct file *dirent_find_file(struct dirent *dir, const char *filename);

extern struct dirent *opendir(const char *dirname);
extern int closedir(struct dirent *dir);
extern struct dirent *readdir(struct dirent *dirp);
extern struct dirent *readdir_r(struct dirent *dir, struct dirent *entry,
		struct dirent **result);
CDECL_END

#endif

