/*
 *  ETA/OS - LibC <string.h>
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#ifndef __STRING_H__
#define __STRING_H__

#include <etaos/types.h>

CDECL

extern char *strchr(const char *str, int c);
extern size_t strlen(const char *str);
extern int strnlen(const char *str, size_t size);
extern int strcmp(const char *s1, const char *s2);
extern void *memset(void *dst, int c, size_t n);
extern char *strcat(char *src, const char *dst);

CDECL_END

#endif
