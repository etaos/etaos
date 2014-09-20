/*
 *  Eta/OS - ETA/OS stdio header
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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

#ifndef __STDIO_H__
#define __STDIO_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

typedef struct file {
	struct file *next;

	unsigned long flags;
	int fd;

	int (*close)(struct file*);
	int (*read)(struct file*, void*, size_t);
	int (*write)(struct file*, void*, size_t);
	int (*flush)(struct file*);

	void *data;
	volatile unsigned char *buff;
	size_t length;
	size_t index;

} *FILE;

extern int putc(int c, FILE stream);
extern int fputc(int c, FILE stream);

#endif
