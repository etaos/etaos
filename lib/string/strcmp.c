/*
 *  ETA/OS - String Compare
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

/** @file strcmp.c */

#include <etaos/kernel.h>
#include <etaos/string.h>

/**
 * @addtogroup libc
 * @{
 */

/**
 * @brief Compare two strings.
 * @param s1 String 1.
 * @param s2 String 2.
 * @return An integer greater than, equal to or less than 0, according to
 *         whether the first string is greater than, equal to or less than the
 *         second string.
 */
int strcmp(const char *s1, const char *s2)
{
	while(*s1 == *s2++) {
		if(*s1++ == 0)
			return 0;
	}

	return (*(unsigned char *) s1 - *(unsigned char *) --s2);
}

/** @} */

