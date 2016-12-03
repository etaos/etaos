/*
 *  ETA/OS - fputs
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

/** @file fputs.c */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/string.h>

/**
 * @addtogroup libcio
 * @{
 */

/**
 * @brief Write a string to a stream.
 * @param s String to write.
 * @param stream Stream to write to.
 * @note Uses fputc internally
 * @return An error code.
 */
int fputs(char *s, struct file * stream)
{
	write(stream->fd, s, strlen(s));
	
	return 0;
}

/** @} */

