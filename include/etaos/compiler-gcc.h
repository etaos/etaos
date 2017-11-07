/*
 *  Eta/OS - GCC compiler defs
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

#ifndef __COMPILER_GCC_H__
#define __COMPILER_GCC_H__

#define __compiler_offsetof(a,b) __builtin_offsetof(a,b)

#define barrier() __asm__ __volatile("" ::: "memory");

#define __hot __attribute__((__hot__))
#define __used __attribute__((used))

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#define compiler_NaN __builtin_nanf("")

#define weak_sym __attribute__((weak))

#endif
