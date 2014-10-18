/*
 *  ETA/OS - STDIO open
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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
#include <etaos/error.h>
#include <etaos/vfs.h>

/**
 * @ingroup libc
 * @brief Open a file on the virtual file system.
 * @param name File name to look for.
 * @param flags File flags.
 */
int open(const char *name, unsigned long flags)
{
	FILE file;

	file = vfs_find(name);
	if(file) {
		file->flags = flags;
		iob_add(file);
		return file->fd;
	}

	return -EINVAL;
}
