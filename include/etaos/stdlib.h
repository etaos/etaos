/*
 *  Eta/OS - ETA/OS stdlib header
 *  Copyright (C) 2014, 2015   Michel Megens <dev@bietje.net>
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

#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <etaos/kernel.h>
#include <etaos/mem.h>
#include <etaos/types.h>
#include <asm/io.h>

CDECL
extern void srand(uint32_t seed);
extern uint32_t random_m(uint32_t max);
extern uint32_t random(void);
extern uint32_t random_r(uint32_t *sptr, uint32_t max);

extern long strtol(const char *nptr, char **endptr, int base);
extern long atol(const char *str);
extern int atoi(const char *str);
extern int abs(int num);
extern long labs(long num);

extern char *ltoa(long num, char *str, int base);
extern char *itoa(int num, char *str, int base);
CDECL_END

#define malloc(__num) kmalloc(__num)
#define zalloc(__num) kzalloc(__num)
#define realloc(__old, __new) krealloc(__old, __new)
#define calloc(__s1, __s2) kcalloc(__s1, __s2)

#ifdef CONFIG_MM_DEBUG
#define free(__p) mm_free(__p, __FILE__, __LINE__)
#else
#define free(__num)   kfree(__num)
#endif

#endif /* __STDLIB_H__ */

