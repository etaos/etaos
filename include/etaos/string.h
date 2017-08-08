/*
 *  ETA/OS - LibC <string.h>
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#ifndef __STRING_H__
#define __STRING_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

#include <asm/io.h>
#include <asm/pgm.h>

CDECL

extern void *memchr(const void *s, int c, size_t n);
extern void *memcpy(void *dst, const void *src, size_t length);
extern int   memcmp(const void *r1, const void *r2, size_t nbytes);
extern char *strchr(const char *str, int c);
extern size_t strlen(const char *str);
extern int strnlen(const char *str, size_t size);
extern int strcmp(const char *s1, const char *s2);
extern void *memset(void *dst, int c, size_t n);
extern char *strcat(char *src, const char *dst);
extern char *strncat(char *dst, const char *src, size_t size);
extern char *strcpy(char *dst, const char *src);
extern char *strncpy(char *dst, const char *src, size_t num);
extern char *strncpy_P(char *dst, const char *src, size_t num);
extern char *strtok_r(char *s, const char *delimeter, char **last);
extern char *strtok(char *s, const char *delimeter);
extern char *strdup(const char *s);

extern int stricmp(const char *s1, const char *s2);
extern int strnicmp(const char *s1, const char *s2, int n);

CDECL_END

#ifndef __HARVARD__
/**
 * @ingroup libc
 * @def memcpy_P
 * @brief Copy memory from a one location to another.
 * @param x Destination address.
 * @param y Source address.
 * @param z Number of bytes to copy.
 *
 * Not all architectures support the 'P' version of memcpy (i.e. not all
 * architectures can copy memory from program space to data space). If this
 * operation is not supported, this function will redirect to the standard
 * memcpy implementation.
 */
#define memcpy_P(x,y,z) memcpy(x,y,z)
#define strlen_P(c) strlen(c)
#define PSTR(s) (s)
#endif

#endif
