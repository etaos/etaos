/*
 *  ETA/OS - LibC readdir
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
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/dirent.h>
#include <etaos/vfs.h>
#include <etaos/error.h>

/**
 * @brief Read from a directory stream.
 * @param dirp Directory stream.
 * @return The next directory entry.
 * @note This function is not thread safe.
 * @see readdir_r
 */
struct dirent *readdir(struct dirent *dirp)
{
	static struct dirent *entry = NULL;
	static struct dirent *result = NULL;

	entry = readdir_r(dirp, entry, &result);
	return result;
}

/** @} */

