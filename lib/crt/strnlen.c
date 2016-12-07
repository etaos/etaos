/*
 *  ETA/OS - LibC strnlen
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/string.h>

/**
 * @addtogroup libc
 * @{
 */

/**
 * @brief Determine the length of the string, while using a maximum.
 * @param str String to determine the length of.
 * @param size Maximum amount of characters to check in \p str.
 * @return An integer smaller than or equal to size, according to whether
 *         the length of \p str is smaller than equal to or greater than
 *         \p size.
 */
int strnlen(const char *str, size_t size)
{
	size_t slen = (size_t)strlen(str);

	if(slen > size)
		return size;
	else
		return slen;
}

/** @} */

