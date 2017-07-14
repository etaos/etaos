/*
 *  Eta/OS - ETA/OS stdio header
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

int snprintf(char *s, size_t length, const char *fmt, ...)
{
	va_list valist;
	struct file f;
	int rv;

	raw_vfs_init_buffered_file(&f, length-1, s);

	va_start(valist, fmt);
	rv = vfprintf(&f, fmt, valist);
	va_end(valist);

	s[f.index] = '\0';
	return rv;
}

