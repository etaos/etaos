/*
 *  ETA/OS - LibC fgetc
 *  Copyright (C) 2014   Michel Megens
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
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>

/**
 * @brief Read a string from a file.
 * @param buf Buffer to store read data into.
 * @param num Maximum number of bytes to read.
 * @param stream File to read from.
 * @return The string read from \p stream.
 *
 * This function reads from the given file until a new line character is
 * read or \p num bytes have been read, whichever comes first.
 */
char *fgets(char *buf, int num, struct file *stream)
{
	int c;
	char *buffer = buf;

	if(!buf)
		return NULL;

	while(num-- > 1) {
		c = fgetc(stream);
		if(c == '\r')
			c = '\n';

		if(c == -EOF) {
			*buffer = '\0';
			return NULL;
		}

		*buffer++ = (char)c;
		if(c == '\n')
			break;
	}

	*buffer = '\0';
	return buf;
}

/** @} */

