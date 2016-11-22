/*
 *  ETA/OS - VFS close
 *  Copyright (C) 2012   Michel Megens
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
 * @addtogroup vfs
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

/**
 * @brief Close a file.
 * @param fd File descriptor to close.
 */
void close(int fd)
{
	vfs_close(fd);
}

/** @} */

