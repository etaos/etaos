/*
 *  ETA/OS - Directory entry
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
 * @addtogroup vfs
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/stdlib.h>
#include <etaos/vfs.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/list.h>

#include <etaos/fs/util.h>

/**
 * @brief Create a new directory entry.
 * @param name Name of the directory.
 * @return The created directory.
 * @retval NULL if the drectory could not be created.
 */
struct dirent *dirent_create(const char *name)
{
	struct dirent *dir;
	int len;

	dir = kzalloc(sizeof(*dir));
	len = strlen(name) + 1;

	list_head_init(&dir->entry);
	list_head_init(&dir->children);

	dir->name = kzalloc(len);
	memcpy(dir->name, name, len);

	return dir;
}

/**
 * @brief Find a directory entry.
 * @param root File system tree to search in.
 * @param path Path to the directory.
 * @return The directory pointed to by \p path.
 * @retval NULL if the directory could not be found.
 */
struct dirent *dirent_find(struct dirent *root, const char *path)
{
	int idx;
	char **sections;
	struct list_head *carriage;
	struct dirent *search;
	bool found = false;

	if(!root || !path)
		return NULL;

	if(!strcmp(root->name, path))
		return root;

	sections = fs_split_path(path);
	search = root;
	for(idx = 0; sections[idx]; idx++) {
		found = false;
		list_for_each(carriage, &search->children) {
			root = search;
			search = list_entry(carriage, struct dirent, entry);

			if(!strcmp(search->name, sections[idx])) {
				found = true;
				break;
			}
		}

	}

	fs_free_path_split(sections);
	if(!found)
		return NULL;

	return search;
}

/**
 * @brief Add a directory as a child to another directory.
 * @param parent Directory to add \p child to.
 * @param child Directory to add to \p parent.
 * @return \p child is returned on success.
 */
struct dirent *dirent_add_child(struct dirent *parent, struct dirent *child)
{
	if(!parent || !child)
		return NULL;

	child->parent = parent;
	child->fs = parent->fs;
	list_add(&child->entry, &parent->children);
	return child;
}

/**
 * @brief Add a file to a directory entry.
 * @param dir Directory to add \p file to..
 * @param file File to add to \p dir.
 * @return The added file.
 */
struct file *dirent_add_file(struct dirent *dir, struct file *file)
{
	if(!dir || !file)
		return NULL;

	file->next = dir->file_head;
	dir->file_head = file;

	return file;
}

/**
 * @brief Find a file in a directory.
 * @param dir Directory to search.
 * @param filename Filename to search for in \p dir.
 * @return The found file or \p NULL.
 * @note This function doesn't search recursively.
 */
struct file *dirent_find_file(struct dirent *dir, const char *filename)
{
	struct file *carriage;

	if(!dir || !filename)
		return NULL;

	carriage = dir->file_head;
	while(carriage) {
		if(!strcmp(carriage->name, filename))
			return carriage;

		carriage = carriage->next;
	}

	return NULL;
}

/**
 * @brief Remove a file from directory.
 * @param dir Directory to remove from.
 * @param file File to remove.
 * @return The removed file or \p NULL.
 * @note This function doesn't search recursively.
 */
struct file *dirent_remove_file(struct dirent *dir, struct file *file)
{
	struct file **fpp;
	struct file *carriage;

	if(!dir || !file)
		return NULL;

	fpp = &dir->file_head;
	while(*fpp) {
		carriage = *fpp;
		if(carriage == file) {
			*fpp = carriage->next;
			return file;
		}

		fpp = &carriage->next;
	}

	return NULL;
}

/**
 * @brief Free an empty directory.
 * @param dir Directory to free.
 * @return An error code.
 * @retval -EINVAL if directory wasn't empty.
 * @retval -EOK if the directory was succesfully deleted.
 */
int dirent_free(struct dirent *dir)
{
	if(!dir || !list_empty(&dir->children) || dir->file_head)
		return -EINVAL;

	kfree(dir->name);
	kfree(dir);

	return -EOK;
}

/** @} */
