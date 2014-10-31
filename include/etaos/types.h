/*
 *  Eta/OS - ETA/OS types
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

#ifndef __ETAOS_TYPES_H_
#define __ETAOS_TYPES_H_

#include <asm/types.h>

#ifndef __cplusplus
typedef unsigned char bool;
#endif
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef short int16_t;

typedef arch_size_t size_t;
typedef arch_ssize_t ssize_t;

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#endif
