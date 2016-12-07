/*
 *  ETA/OS - LibC ioctl
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

/**
 * @addtogroup libcio
 * @{
 */
#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>

/**
 * @brief Drive file control options.
 * @param stream Stream to set the IOCTL to.
 * @param reg Control option to read from or write to.
 * @param buf Buffer which can be read from or written into.
 * @note Make sure that buf is long enough. See the documentation of the
 *       specific device driver for information on the length of the buffer.
 * @return An error code.
 */
int ioctl(struct file * stream, unsigned long reg, void *buf)
{
	int rc;

	if(!buf && test_bit(STREAM_READ_FLAG, &stream->flags))
		rc = stream->ioctl(stream, reg, buf);
	else if(buf && (stream->flags & (__SRD | __SWR)) == (__SRD | __SWR))
		rc = stream->ioctl(stream, reg, buf);
	else
		rc = -EINVAL;

	return rc;
}

/** @} */

