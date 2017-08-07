/*
 *  ETA/OS - Ohh snap, panic!
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
 * @file include/etaos/panic.h Kernel panic
 */

#ifndef __PANIC_H__
#define __PANIC_H__

#include <etaos/kernel.h>

/**
 * @addtogroup kernel
 * @{
 */

#define ETAOS_PANIC_STRING "[ETA/OS PANIC]:"

extern void panic(const char *fmt, ...);

#ifdef CONFIG_HARVARD
extern void panic_P(const char *fmt, ...);
#else
#define panic_P(__fmt, ...) panic(__fmt, __VA_ARGS__)
#endif /* CONFIG_HARVARD */

/** @} */
#endif

