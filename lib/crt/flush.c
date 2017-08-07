/*
 *  ETA/OS - LibC flush
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

/**
 * @brief Flush a file.
 * @param fd File descriptor to flush.
 * @return An error code.
 */
int flush(int fd)
{
	FILE *file;
	int rc;

	if(fd >= MAX_OPEN)
		return -EBADF;

	file = __iob[fd];
	if(!file)
		return -EBADF;

	if(file->flush)
		rc = file->flush(file);
	else
		rc = -EOK;

	return rc;
}

/** @} */

