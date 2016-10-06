/*
 *  ETA/OS - STDIO printf
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
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
 * @brief Write a formated string to stdout.
 * @param fmt Format string.
 * @param ... Variable argument list.
 * @return Number of bytes written to stdout.
 *
 * The first argument contains a formatted string, for example:
 * @code{.c}
   printf("Hey there, it is %u:%uPM\n", 6, 23);
   @endcode
 *
 * The %u means 'replace with unsigned integer from the variable argument list.
 * What will be written to stdout in the end:
 * Hey there, it is 6:23PM
 */
int printf(const char *fmt, ...)
{
	int i;
	va_list va;

	va_start(va, fmt);
	i = vfprintf(stdout, fmt, va);
	va_end(va);

	return i;
}

/** @} */

