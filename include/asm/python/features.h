/*
 *  ETA/OS - Python header
 *  Copyright (C) 2017  Michel Megens <dev@bietje.net>
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

#ifndef __FEATURES_H__
#define __FEATURES_H__

#include <generated/autoconf.h>

#ifdef CONFIG_HAVE_SLICE
#define HAVE_SLICE
#endif

#ifdef CONFIG_HAVE_PRINT
#define HAVE_PRINT
#endif

#ifdef CONFIG_CLASSES
#define HAVE_CLASSES
#endif

#ifdef CONFIG_HAVE_REPLICATION
#define HAVE_REPLICATION
#endif

#ifdef CONFIG_DEFAULTARGS
#define HAVE_DEFAULTARGS
#endif

#ifdef CONFIG_HAVE_DEL
#define HAVE_DEL
#endif

#ifdef CONFIG_HAVE_BACKTICK
#define HAVE_BACKTICK
#endif

#ifdef CONFIG_HAVE_FLOAT
#define HAVE_FLOAT
#endif

#ifdef CONFIG_HAVE_GC
#define HAVE_GC
#endif

#ifdef CONFIG_HAVE_STRING_FORMAT
#define HAVE_STRING_FORMAT
#endif

#ifdef CONFIG_HAVE_ASSERT
#define HAVE_ASSERT
#endif

#ifdef CONFIG_HAVE_CLOSURES
#define HAVE_CLOSURES
#endif

#ifdef CONFIG_HAVE_GENERATORS
#define HAVE_GENERATORS
#endif

#ifdef CONFIG_HAVE_IMPORTS
#define HAVE_IMPORTS
#endif

#ifdef CONFIG_HAVE_DEBUG_INFO
#define HAVE_DEBUG_INFO
#endif

#ifdef CONFIG_HAVE_BYTEARRAY
#define HAVE_BYTEARRAY
#endif

#define HAVE_AUTOBOX

#endif /* __FEATURES_H__ */

