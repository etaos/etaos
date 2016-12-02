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

/**
 * @addtogroup ramfs
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/vfs.h>
#include <etaos/ramfs.h>
#include <etaos/init.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/spinlock.h>

#include <etaos/fs/basename.h>

static inline void ramfs_lock_file(struct vfile *file)
{
	spin_lock(&file->lock);
}

static inline void ramfs_unlock_file(struct vfile *file)
{
	spin_unlock(&file->lock);
}

/**
 * @brief Open a RAMFS file.
 * @param file File to open.
 * @return An exit code.
 * @retval -EOK on success.
 */
static int ramfs_open(struct vfile *file)
{
	struct ramfs_file *ramfile;

	ramfs_lock_file(file);
	ramfile = container_of(file, struct ramfs_file, base);
	if(test_bit(STREAM_APPEND_FLAG, &file->flags))
		ramfile->wr_idx = file->index;
	else
		file->index = 0UL;

	return -EOK;
}

/**
 * @brief Close RAMFS file.
 * @param file File to close.
 * @return Exit code.
 * @retval -EOK on success.
 */
static int ramfs_close(struct vfile *file)
{
	struct ramfs_file *ramfile = container_of(file, struct ramfs_file, base);

	ramfile->rd_idx = 0UL;
	ramfile->wr_idx = 0UL;
	ramfs_unlock_file(file);

	return -EOK;
}

/**
 * @brief Expand the a RAMFS file as required.
 * @param file File to expend.
 * @param required Minimum number of additional bytes required.
 * @return An error code.
 * @retval -ENOMEM if the reallocation of th file buffer failed.
 * @retval -EOK on success.
 */
static int ramfs_expand(struct vfile *file, size_t required)
{
	size_t expand;
	struct ramfs_file *ramfile;
	void *tmp;

	ramfile = container_of(file, struct ramfs_file, base);
	
	if(ramfile->wr_idx + required >= file->length) {
		expand = file->length + 1;
		expand += ramfile->wr_idx + required - file->length;
		/*
		 * Reallocate the file buffer using realloc.
		 */
		tmp = krealloc((void*)file->buff, expand);

		if(!tmp)
			return -ENOMEM;

		file->buff = tmp;
	}

	return -EOK;
}

/**
 * @brief Write to a RAMFS file.
 * @param file File to write to.
 * @param buff Buffer containing data to write to \p file.
 * @param size Length of \p buff.
 * @return The number of bytes written.
 */
static int ramfs_write(struct vfile *file, const void *buff, size_t size)
{
	struct ramfs_file *ramfile;
	const char *cbuff = buff;
	size_t idx = 0UL;

	ramfile = container_of(file, struct ramfs_file, base);

	if(unlikely(ramfs_expand(file, size)))
		return 0;

	while(idx < size && ramfile->wr_idx < file->length)
		file->buff[ramfile->wr_idx++] = cbuff[idx++];

	file->index += size;
	return size;
}

/**
 * @brief Get the currenct index of a RAMFS file.
 * @param file File to get the current index of.
 * @return The current index of \p file.
 */
static size_t ramfs_ftell(struct vfile *file)
{
	struct ramfs_file *ramfile;

	ramfile = container_of(file, struct ramfs_file, base);
	return ramfile->wr_idx;
}

static size_t ramfs_setindex(struct vfile *file, size_t index, size_t max)
{
	struct ramfs_file *ramfile;

	if(index > max && max)
		return -EINVAL;

	if(index != file->index) {
		ramfile = container_of(file, struct ramfs_file, base);
		file->index = index;
		ramfile->rd_idx = 0;
		ramfile->wr_idx = 0;
	}

	return index;
}

/**
 * @brief Set the index to a specific position.
 * @param file RAMFS file.
 * @param offset Offset to set in \p file.
 * @param whence Operation type.
 * @return The new index of \p file.
 */
static size_t ramfs_lseek(struct vfile *file, size_t offset, int whence)
{
	switch(whence) {
	case SEEK_END:
		offset += file->length;
		break;

	case SEEK_CUR:
		if(!offset)
			return file->index;

		return ramfs_setindex(file, file->index + offset, 0);
	default:
		break;
	}

	return ramfs_setindex(file, offset, 0);
}

/**
 * @brief Read from a RAMFS file.
 * @param file File to read from.
 * @param buff Buffer to store the read data into.
 * @param size Number of bytes to read.
 * @return The number of bytes read.
 */
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

/**
 * @brief Read a single byte from a RAMFS file.
 * @param file File to read from.
 * @return The character read.
 */
static int ramfs_getc(struct vfile *file)
{
	struct ramfs_file *ramfile;

	ramfile = container_of(file, struct ramfs_file, base);
	return file->buff[ramfile->rd_idx++];
}

/**
 * @brief Write a single character to a RAMFS file.
 * @param c Character to write.
 * @param file File to write to.
 * @return The character written, \p c.
 */
static int ramfs_putc(int c, struct vfile *file)
{
	struct ramfs_file *ramfile;

	if(unlikely(ramfs_expand(file, 1)))
		return 0;

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

	spinlock_init(&file->base.lock);
	file->base.write = ramfs_write;
	file->base.read  = ramfs_read;
	file->base.put   = ramfs_putc;
	file->base.get   = ramfs_getc;
	file->base.close = ramfs_close;
	file->base.open  = ramfs_open;
	file->base.ftell = ramfs_ftell;
	file->base.lseek = ramfs_lseek;
	file->base.name  = basen;
	file->base.buff  = kzalloc(RAMFS_BUFFER_SIZE);
	file->base.length = RAMFS_BUFFER_SIZE;

	vfs_add_file(basep, &file->base);
	kfree(basep);

	return &file->base;
}

/**
 * @brief RAMFS driver.
 */
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

/** @} */

