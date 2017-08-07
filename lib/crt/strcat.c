/*
 *  ETA/OS - String concatinate
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
#include <etaos/string.h>

/**
 * @brief Concatinate two strings.
 * @param dst Destination string.
 * @param src Source string.
 * @return The destination string.
 *
 * The string \p src will be appended to the \p dst string.
 */
char *strcat(char *dst, const char *src)
{
	char *rv = dst;

	for(; *dst; ++dst);
	while(*src != '\0')
		*dst++ = *src++;

	return rv;
}

/**
 * @brief Concatinate two strings.
 * @param dst Destination string.
 * @param src Source string.
 * @param size Number of characters to append.
 * @return The destination string.
 *
 * The string \p src will be appended to the \p dst string.
 */
char *strncat(char *dst, const char *src, size_t size)
{
	char *d = dst;
	const char *s = src;

	if(!size)
		return dst;

	while(*d != '\0')
		d++;

	do {
		if((*d = *s++) == '\0')
			break;
		d++;
	} while(--size);

	*d = '\0';
	return dst;
}

/** @} */

