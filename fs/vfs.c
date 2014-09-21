/*
 *  ETA/OS - VFS driver.
 *  Copyright (C) 2012   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

FILE __iob[MAX_OPEN];
static struct file *vfshead;

int iob_add(FILE stream)
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

void vfs_init(void)
{
	int i = 3;

	for(; i < MAX_OPEN; i++)
		__iob[i] = NULL;

	vfshead = NULL;
}

void vfs_add(FILE newfile)
{
	newfile->next = vfshead;
	vfshead = newfile;
}

int vfs_delete(FILE file)
{
	struct file **fpp;

	fpp = &file;
	for(; *fpp; fpp = &(*fpp)->next) {
		if(*fpp == file) {
			*fpp = file->next;
			return 0;
		}
	}

	return -1;
}

