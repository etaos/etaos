/*
 *  ETA/OS - Device ops
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

#ifndef __PYTHON_DEVICE_H__
#define __PYTHON_DEVICE_H__

#include <etaos/kernel.h>

CDECL
extern int device_write(const uint8_t *path, const void *buffer, size_t num, size_t *addr);
extern int device_read(const uint8_t *path, void *buffer, size_t num, size_t *addr);
CDECL_END

#endif /* end of include guard: __PYTHON_DEVICE_H__ */
