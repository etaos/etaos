/*
 *  ETA/OS - VFS driver.
 *  Copyright (C) 2012, 2016   Michel Megens <dev@bietje.net>
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
 * @addtogroup devfs
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>
#include <etaos/devfs.h>

/**
 * @brief Send an IOCTL to the device file system.
 * @param file Device file.
 * @param reg IOCTL register.
 * @param arg Argument to \p reg.
 * @return An error code.
 * @retval -EINVAL when an error has occurred.
 * @retval -EOK on success.
 */
static int devfs_ioctl(struct file *file, unsigned long reg, void *arg)
{
	return -EINVAL;
}

/**
 * @brief Device file system driver.
 */
struct fs_driver devfs = {
	.ioctl = devfs_ioctl,
};

void devfs_init(void)
{
}

/**
 * @}
 */

