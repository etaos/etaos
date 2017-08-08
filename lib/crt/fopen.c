/*
 *  ETA/OS - fopen()
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
 * @addtogroup libcio
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

/**
 * @brief Open a file on the virtual file system.
 * @param name File name to look for.
 * @param modes File modes.
 * @return The opened file pointer.
 */
FILE *fopen(const char *name, const char *modes)
{
	unsigned long flags;
	int fd;

	flags = fmode(modes);

	if(flags == -EOF)
		return NULL;

	fd = open(name, flags);
	if(fd < 0)
		return NULL;

	return filep(fd);
}

/** @} */

