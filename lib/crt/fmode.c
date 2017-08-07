/*
 *  ETA/OS - fmode()
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
 * @brief Convert a filemode string to a bitfield.
 * @param modes Filemode to convert.
 * @return The numeral filemode for \p modes.
 */
unsigned long fmode(const char *modes)
{
	unsigned long mflags, tmp;

	switch(*modes) {
	case 'r':
		mflags = O_RDONLY;
		tmp = O_RDWR;
		break;

	case 'w':
		mflags = O_WRONLY | O_CREAT | O_TRUNC;
		tmp = O_RDWR | O_CREAT | O_TRUNC;
		break;

	case 'a':
		mflags = O_WRONLY | O_CREAT | O_APPEND;
		tmp = O_RDWR | O_CREAT | O_APPEND;
		break;
		
	default:
		return -EOF;
	}

	while(*++modes) {
		switch(*modes) {
		case '+':
			mflags = tmp;
			break;

		case 'b':
			break;

		default:
			return -EOF;
		}
	}

	return mflags;
}

/** @} */

