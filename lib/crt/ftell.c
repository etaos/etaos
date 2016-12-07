/*
 *  ETA/OS - LibC ftell
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
 * @addtogroup libcio
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/error.h>

/**
 * @brief Get the current position of a stream.
 * @param file File (stream) to get the current position of.
 * @return The current position of the stream.
 */
size_t ftell(struct file *file)
{
	if(file->ftell)
		return file->ftell(file);

	return file->index;
}

/** @} */

