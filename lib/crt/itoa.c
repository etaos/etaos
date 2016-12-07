/*
 *  ETA/OS - LibC itoa
 *  Copyright (C) 2016  Michel Megens <dev@bietje.net>
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
#include <etaos/stdlib.h>

/**
 * @brief Convert an integer to a null-terminated string.
 * @param num Numeral value to convert.
 * @param str Array in memory to store the resulting string.
 * @param base Numerical base used to represent \p value in.
 * @return The converted integer value.
 */
char *itoa(int num, char *str, int base)
{
	return ltoa((long)num, str, base);
}

/** @} */

