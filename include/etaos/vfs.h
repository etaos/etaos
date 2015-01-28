/*
 *  ETA/OS - VFS driver.
 *  Copyright (C) 2012, 2014, 2015   Michel Megens
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

#ifndef __VFS_H__
#define __VFS_H__

#include <etaos/kernel.h>
#include <etaos/stdio.h>

CDECL

extern void vfs_init(void);
extern void vfs_add(FILE file);
extern int vfs_delete(FILE f);
extern FILE vfs_find(const char *name);

CDECL_END

#endif
