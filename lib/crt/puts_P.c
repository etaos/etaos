/*
 *  ETA/OS - puts_P()
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
 * @addtogroup libcio
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/error.h>
#include <asm/pgm.h>

int puts_P(const char *string)
{
	char *str;
	size_t strl;

	strl = strlen_P(string) + 1;
	str = kzalloc(strl);

	if(!str)
		return -ENOMEM;

	memcpy_P(str, string, strl);
	puts(str);
	kfree(str);

	return -EOK;
}

/** @} */
