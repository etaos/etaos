/*
 *  ETA/OS - strncpy_P()
 *  Copyright (C) 2015   Michel Megens <dev@bietje.net>
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
#include <etaos/stdlib.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <asm/pgm.h>

char *strncpy_P(char *dst, const char *src, size_t num)
{
	char *_src;
	char *rv;

	_src = kzalloc(num + 1);
	if(_src == NULL)
		return NULL;
	
	memcpy_P(_src, src, num);
	rv = strncpy(dst, _src, num);
	kfree(_src);
	return rv;
}

/** @} */

