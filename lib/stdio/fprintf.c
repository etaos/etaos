/*
 *  ETA/OS - STDIO printf
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

/** @file printf.c */

#include <etaos/kernel.h>
#include <etaos/stdio.h>

/**
 * @addtogroup libcio
 * @{
 */

/**
 * @brief Write a formated string to a stream.
 * @param stream File to write to.
 * @param fmt Format string.
 * @param ... Variable argument list.
 * @return Number of bytes written to \p stream.
 *
 * The first argument contains a formatted string, for example:
 * @code{.c}
   printf(stderr, "Hey there, it is %u:%uPM\n", 6, 23);
   @endcode
 *
 * The %u means 'replace with unsigned integer from the variable argument list.
 * What will be written to the file in the end:
 * Hey there, it is 6:23PM
 */
int fprintf(struct vfile * stream, const char *fmt, ...)
{
	int i;
	va_list va;

	va_start(va, fmt);
	i = vfprintf(stream, fmt, va);
	va_end(va);

	return i;
}

/** @} */

