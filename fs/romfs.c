/*
 *  ETA/OS - ROMFS driver
 *  Copyright (C) 2015   Michel Megens <dev@michelmegens.net>
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

/**
 * @addtogroup romfs
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdlib.h>
#include <etaos/init.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/vfs.h>
#include <etaos/romfs.h>
#include <etaos/stdio.h>

/**
 * @brief Open a ROMFS file.
 * @param file File which has to be opened.
 * @return Error code.
 * @retval -EXIST
 * @retval -EOK
 *
 * This file copies all data belonging to this file into data memory for
 * quick access by read and getc.
 */
static int romfs_open(struct vfile *file)
{
        char *buff;
        struct romfs *entry;

        if(file->data)
                return -EEXIST;

        entry = file->fs_data;
        buff = kzalloc(file->length);
        memcpy_P(buff, entry->data, file->length);

        file->data = buff;
        atomic_inc(&file->uses);

        return -EOK;
}

/**
 * @brief Close a ROMFS file.
 * @param file File to close.
 * @return Error code.
 * @retval -EOK
 */
static int romfs_close(struct vfile *file)
{
	int uses;

	atomic_dec(&file->uses);
	uses = atomic_get(&file->uses);

	if(!uses) {
		kfree(file->data);
		file->data = NULL;
		file->index = 0;
	}

	return -EOK;
}

/**
 * @brief Read \p len amount of bytes from a ROMFS file.
 * @param file File to read from.
 * @param buff Buffer to store the read data in.
 * @param len Amount of bytes to read.
 * @return The amount of bytes read.
 */
static int romfs_read(struct vfile *file, void *buff, size_t len)
{
	size_t readable;

	if(file->index >= file->length)
		return -EOF;

	if(len > (file->length - file->index))
		readable = file->length - file->index;
	else
		readable = len;

	memcpy(buff, file->data + file->index, readable);
	file->index += readable;

	return (int)readable;
}

/**
 * @brief Read a single character from a ROMFS file.
 * @param file File to read from.
 * @return The character read from the file buffer.
 */
static int romfs_getc(struct vfile *file)
{
	int c;
	char *data;

	if(file->index >= file->length)
		return -EOF;

	data = file->data;
	c = data[file->index];
	file->index++;

	return c;
}

/**
 * @brief Initialise a new file based on a ROMFS entry.
 * @param file File which has to be initialised.
 * @param entry ROMFS entry to use as a base for a file.
 */
static void romfs_setup_file(struct vfile *file, struct romfs *entry)
{
	file->name = entry->name;
	file->length = entry->length - 1; /* do not include EOF */
	file->fs_data = entry;
	spinlock_init(&file->lock);
	atomic_init(&file->uses);

	file->read = &romfs_read;
	file->get = &romfs_getc;
	file->open = &romfs_open;
	file->close = &romfs_close;
}

static void __used romfs_rollout(void)
{
	struct romfs *entry;
	struct vfile *file;

	for(entry = romEntryList; entry; entry = entry->next) {
		file = kzalloc(sizeof(*file));
		if(!file)
			return;

		romfs_setup_file(file, entry);
		vfs_add(file);
	}
}

module_init(romfs_rollout);

/** @} */

