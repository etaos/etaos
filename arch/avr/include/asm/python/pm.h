/*
 *  ETA/OS - Python arch header
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

#ifndef __PYTHON_AVR_H__
#define __PYTHON_AVR_H__

#include <etaos/kernel.h>

#define PM_PLAT_POINTER_SIZE 2
#define PM_FLOAT_LITTLE_ENDIAN 1

typedef enum port_type {
	PORT,
	PIN,
	DDR,
} port_type_t;

CDECL
extern volatile unsigned char *pm_port_to_addr(int32_t port, port_type_t type);
CDECL_END

#endif

