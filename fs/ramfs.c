/*
 *  ETA/OS - RAMFS driver
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

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/vfs.h>
#include <etaos/ramfs.h>
#include <etaos/init.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>

#include <etaos/fs/basename.h>

static int ramfs_open(struct vfile *file)
{
	struct ramfs_file *ramfile;

	ramfile = container_of(file, struct ramfs_file, base);
	if(test_bit(STREAM_APPEND_FLAG, &file->flags))
		ramfile->wr_idx = file->index;
	else
		file->index = 0UL;

	return -EOK;
}

static int ramfs_close(struct vfile *file)
{
	struct ramfs_file *ramfile = container_of(file, struct ramfs_file, base);

	ramfile->rd_idx = 0UL;
	ramfile->wr_idx = 0UL;

	return -EOK;
}

static int ramfs_write(struct vfile *file, const void *buff, size_t size)
{
	struct ramfs_file *ramfile;
	const char *cbuff = buff;
	size_t idx = 0UL;

	ramfile = container_of(file, struct ramfs_file, base);
	
	while(idx < size && ramfile->wr_idx < file->length)
		file->buff[ramfile->wr_idx++] = cbuff[idx++];

	file->index += size;
	return size;
}

static int ramfs_read(struct vfile *file, void *buff, size_t size)
{
	struct ramfs_file *ramfile;
	char *cbuff = buff;
	size_t idx = 0UL;

	ramfile = container_of(file, struct ramfs_file, base);

	while(idx < size && ramfile->rd_idx < file->length)
		cbuff[idx++] = file->buff[ramfile->rd_idx++];

	return size;
}

static int ramfs_getc(struct vfile *file)
{
	struct ramfs_file *ramfile;

	ramfile = container_of(file, struct ramfs_file, base);
	return file->buff[ramfile->rd_idx++];
}

static int ramfs_putc(int c, struct vfile *file)
{
	struct ramfs_file *ramfile;

	ramfile = container_of(file, struct ramfs_file, base);
	file->buff[ramfile->wr_idx++] = c;
	file->index++;

	return c;
}

#define RAMFS_BUFFER_SIZE 64

/**
 * @brief Create a new RAMFS file
 * @param path Path to the file.
 * @param mode File mode.
 * @note \p path contains the full path to the file.
 * @return The created RAMFS file.
 */
static struct vfile *ramfs_create(const char *path, int mode)
{
	struct ramfs_file *file;
	char *basep, *basen;

	file = kzalloc(sizeof(*file));

	if(!file)
		return NULL;

	basep = basepath(path);
	basen = basename(path);

	file->base.write = ramfs_write;
	file->base.read  = ramfs_read;
	file->base.put   = ramfs_putc;
	file->base.get   = ramfs_getc;
	file->base.close = ramfs_close;
	file->base.open  = ramfs_open;
	file->base.name  = basen;
	file->base.buff  = kzalloc(RAMFS_BUFFER_SIZE);
	file->base.length = RAMFS_BUFFER_SIZE;

	vfs_add_file(basep, &file->base);

	return &file->base;
}

struct fs_driver ramfs = {
	.open = ramfs_create,
	.close = ramfs_close,
	.write = ramfs_write,
	.read = ramfs_read,
	.get = ramfs_getc,
	.put = ramfs_putc,
};

void __used ramfs_init(void)
{
	/*
	 * Create the RAMFS directory and mount point
	 */
	mkdir("/ramfs");
	mount(&ramfs, "/ramfs");
}

module_init(ramfs_init);

