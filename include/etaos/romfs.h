/*
 *  ETA/OS - ROMFS header
 *  Copyright (C)  2015   Michel Megens
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

#ifndef __ROMFS_H__
#define __ROMFS_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

#include <asm/pgm.h>

/**
 * @brief Micro ROM filesystem entry.
 */
struct romfs {
	struct romfs *next; //!< Next pointer.
	const char *name;   //!< File name of the entry.
	size_t length;      //!< Length of the \p data.
	const char *data;   //!< Data saved by the entry.
};

extern struct romfs *romEntryList;

#endif
