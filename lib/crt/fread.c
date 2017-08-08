/*
 *  ETA/OS - LibC fread
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
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/bitops.h>
#include <etaos/vfs.h>

/**
 * @brief Read from a file.
 * @param file File to read from.
 * @param buff Buffer to store data into.
 * @param len Size of \p buff.
 * @return Number of bytes read or an error code.
 */
int fread(FILE *file, void *buff, size_t len)
{
	if(!file || !buff)
		return -EINVAL;

	return vfs_read(file, buff, len);
}

/** @} */

