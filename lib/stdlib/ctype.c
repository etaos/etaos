/*
 *  ETA/OS - LibC ctype
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
#include <etaos/types.h>
#include <etaos/ctype.h>

/**
 * @brief Check if \p c is upper case.
 * @param c Character to check.
 * @return Boolean result.
 */
int isupper(int c)
{
    return (c >= 'A' && c <= 'Z');
}

/**
 * @brief Check if \p c is alphabetic.
 * @param c Character to check.
 * @return Boolean result.
 */
int isalpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/**
 * @brief Check if \p c is a space, tab or newline feed.
 * @param c Character to check.
 * @return Boolean result.
 */
int isspace(int c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\12');
}

/**
 * @brief Check if \p c is a digit.
 * @param c Character to check.
 * @return Boolean result.
 */
int isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

/** @} */

