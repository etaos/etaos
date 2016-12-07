/*
 *  ETA/OS - LibC atoi
 *  Copyright (C) 2004 by Egnite Software GmbH
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
 * @brief Convert a string to an integer.
 * @param str The string.
 * @return Integer value.
 * @see strtol
 */
int atoi(const char *str)
{
	return ((int) strtol(str, (char **) NULL, 10));
}

/**
 * @brief Convert a string to a long integer.
 * @param str The string.
 * @return Long integer value.
 * @see strtol
 */
long atol(const char *str)
{
	return strtol(str, (char**)NULL, 10);
}

/** @} */

