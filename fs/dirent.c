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

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/stdlib.h>
#include <etaos/vfs.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/list.h>

#include <etaos/fs/util.h>

struct dirent *dirent_create(const char *name)
{
	struct dirent *dir;
	int len;

	dir = kzalloc(sizeof(*dir));
	len = strlen(name) + 1;

	list_head_init(&dir->entry);
	list_head_init(&dir->children);

	dir->name = kzalloc(len);
	memcpy(dir->name, name, len+1);

	return dir;
}

struct dirent *dirent_find(struct dirent *root, const char *path)
{
	int idx;
	char **sections;
	struct list_head *carriage;
	struct dirent *search;
	bool found = false;

	if(!root || !path)
		return NULL;

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

struct dirent *dirent_add_child(struct dirent *parent, struct dirent *child)
{
	child->parent = parent;
	list_add(&child->entry, &parent->children);
	return child;
}

struct vfile *dirent_add_file(struct dirent *dir, struct vfile *file)
{
	if(!dir || !file)
		return NULL;

	file->next = dir->file_head;
	dir->file_head = file;

	return file;
}

struct vfile *dirent_remove_file(struct dirent *dir, struct vfile *file)
{
	struct vfile **fpp;
	struct vfile *carriage;

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

int dirent_free(struct dirent *dir)
{
	if(!dir || !list_empty(&dir->children) || dir->file_head)
		return -EINVAL;

	kfree(dir->name);
	kfree(dir);

	return -EOK;
}

