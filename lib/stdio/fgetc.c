/*
 *  ETA/OS - LibC fgetc
 *  Copyright (C) 2014   Michel Megens
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

/**
 * @addtogroup libcio
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>

/**
 * @brief Write a single byte from a file.
 * @param stream Stream to write from.
 */
int fgetc(struct vfile * stream)
{
	if(test_bit(STREAM_READ_FLAG, &stream->flags) && stream->get)
		return stream->get(stream);
	else
		return -EINVAL;
}

/** @} */

