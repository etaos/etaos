/*
 *  ETA/OS - Driver core
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
 * @file etaos/device.h Device header
 * @addtogroup dev-core
 * @{
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/mutex.h>

/**
 * @brief Platform device structure.
 *
 * Platform devices describe a device which is specific
 * to a single platform.
 */
struct platform_device {
	const char *name;
	void *io_base;
};

/**
 * @brief Device driver descriptor.
 */
struct device {
	/** @brief Name of the device */
	const char *name;
	/** @brief Device list */
	struct list_head devs;
	/** @brief Device file descriptor */
	struct file file;
	/** @brief Mutex to ensure exclusive access */
	mutex_t dev_lock;
	/** @brief Backend platform device */
	struct platform_device *pdev;
	/** @brief Private data pointer */
	void *dev_data;

	/**
	 * @brief Device free/unregister
	 * @param dev Device to free.
	 */
	void (*release)(struct device *dev);
};

/**
 * @struct dev_file_ops
 * @brief Device file operations.
 *
 * Description of all available file operations for devices. This structure
 * is generally used during the setup of a device.
 */
struct dev_file_ops {
	int (*close)(struct file*); //!< File close.
	int (*read)(struct file*, void*, size_t); //!< Read from a file.
	int (*write)(struct file*, const void*, size_t); //!< Write to a file.
	int (*flush)(struct file*); //!< Flush the file.
	int (*put)(int c, struct file*); //!< Write 1 byte to a file.
	int (*get)(struct file*); //!< Read 1 byte from a file.
};

extern struct device *device_create(const char *name, void *data,
		struct dev_file_ops *fops);
extern int device_initialize(struct device *dev, struct dev_file_ops *fops);
extern int dev_register_pdev(struct platform_device *pdev, 
		struct dev_file_ops *fops);
extern void dev_core_init();
extern int dev_set_fops(struct device *dev, struct dev_file_ops *fops);
extern struct device *dev_get_by_name(const char *name);

/**
 * @brief Return the IOBASE of a platform device.
 * @param pdev Platform device to lookup the IOBASE for.
 * @return The IOBASE.
 */
static inline void *pdev_get_iobase(struct platform_device *pdev)
{
	if(pdev)
		return pdev->io_base;

	return NULL;
}

/**
 * @brief Get the IOBASE of a device.
 * @param dev Device to lookup the IOBASE for.
 * @note The given device needs to be a platform device.
 * @warning If the device has no platform device, NULL is returned.
 */
static inline void *dev_get_iobase(struct device *dev)
{
	if(dev)
		return pdev_get_iobase(dev->pdev);

	return NULL;
}

#define dev_to_file(__d) (&(__d)->file)

#endif

/** @} */

