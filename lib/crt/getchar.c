/*
 *  ETA/OS - LibC getchar
 *  Copyright (C) 2017   Michel Megens
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
#include <etaos/stdio.h>

/**
 * @brief Return the next character from the standard input.
 * @return The character read from `stdin`.
 * @note This is the same as calling `fgetc()` with `stdin` as argument.
 */
int getchar(void)
{
	return fgetc(stdin);
}

/** @} */

