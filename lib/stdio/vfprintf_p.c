/*
 *  ETA/OS - vfprintf_p
 *  Copyright (C) 2015   Michel Megens
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
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdlib.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <asm/pgm.h>

/**
 * @brief vfprintf version for progmem strings.
 * @param stream File to write to.
 * @param fmt Format string (stored in program memory).
 * @param ap VA list to complete the format string.
 * @return Number of bytes written to \p stream.
 */
int vfprintf_P(struct vfile * stream, const char *fmt, va_list ap)
{
	size_t strl = strlen_P(fmt) + 1;
	int rc = -ENOMEM;
	char *_fmt;

	_fmt = kzalloc(strl);
	if(_fmt) {
		memcpy_P(_fmt, fmt, strl);
		rc = vfprintf(stream, _fmt, ap);
		kfree(_fmt);
	}

	return rc;
}

/** @} */
