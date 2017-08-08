/*
 *  ETA/OS - LibC fileno
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
#include <etaos/bitops.h>
#include <etaos/error.h>
#include <etaos/stdio.h>

/**
 * @brief Get the file descriptor of a file pointer.
 * @param file File to get the file descriptor for.
 * @return The file descriptor for \p file.
 * @warning This function will not check whether file is a valid stream.
 */
int fileno(FILE *file)
{
	if(!file)
		return -EBADF;

	return file->fd;
}

/** @} */

