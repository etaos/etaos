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

/**
 * @file etaos/types.h
 */

#ifndef __ETAOS_TYPES_H_
#define __ETAOS_TYPES_H_

#include <asm/types.h>

#ifndef __cplusplus
/**
 * @brief C boolean type.
 *
 * Although it is actually a byte width type, it should only be set to
 * \p true or \p false, for C++ compatibility.
 */
typedef unsigned char bool;
#endif

/**
 * @brief Unsigned 8-bit integer.
 */
typedef unsigned char uint8_t;
/**
 * @brief Unsigned 16-bit integer.
 */
typedef unsigned short uint16_t;

/**
 * @brief Signed 8-bit integer
 */
typedef char int8_t;
/**
 * @brief Signed 16-bi integer.
 */
typedef short int16_t;

/**
 * @brief Unsigned pointer width type.
 *
 * The size_t type always has the same width as a void pointer.
 */
typedef arch_size_t size_t;
/**
 * @brief Signed pointer width type.
 * @see size_t
 */
typedef arch_ssize_t ssize_t;

/**
 * @brief Linked list structure.
 * @see list_add list_del
 */
struct list_head {
	struct list_head *next; //!< Next node pointer.
	struct list_head *prev; //!< Previous node pointer.
};

#endif
