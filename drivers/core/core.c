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

/** @file drivers/core/core.c */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/mutex.h>
#include <etaos/device.h>
#include <etaos/list.h>
#include <etaos/device.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/vfs.h>

/**
 * @addtogroup dev-core Device driver core
 *
 * The device driver core is responsible for internal handling of device
 * drivers. Registering, and unregistering them for example
 * @{
 */

static struct list_head dev_root = STATIC_INIT_LIST_HEAD(dev_root);

static int _dev_set_fops(struct device *dev, struct dev_file_ops *fops);
static struct device *dev_allocate(const char *name, struct dev_file_ops *fops);

/**
 * @brief Initialise the device core.
 */
void dev_core_init(void)
{
	list_head_init(&dev_root);
}

static void dev_release(struct device *dev)
{
	if(!dev)
		return;

	list_del(&dev->devs);
	kfree(dev);
}

static inline int dev_name_is_unique(struct device *dev)
{
	struct list_head *carriage;
	struct device *c_dev;

	if(list_empty(&dev_root))
		return -EOK;

	list_for_each(carriage, &dev_root) {
		c_dev = list_entry(carriage, struct device, devs);
		if(!strcmp(dev->name, c_dev->name))
			return -EINVAL;
	}

	return -EOK;
}

/**
 * @brief Create a new device.
 * @param name Name of the new device.
 * @param data Private device data.
 * @param fops Device file operations.
 * @note \p name must be unique.
 */
struct device *device_create(const char *name, void *data,
		struct dev_file_ops *fops)
{
	struct device *dev;

	dev = dev_allocate(name, fops);
	if(!dev)
		return NULL;

	dev->dev_data = data;
	return dev;
}

/**
 * @brief Initialise a device structure.
 * @param dev Device structure to initialise.
 * @param fops File operations.
 * @return Error code.
 * @retval 0 on success.
 * @retval -EINVAL on error.
 */
int device_initialize(struct device *dev, struct dev_file_ops *fops)
{
	int err;

	if(!dev || !dev->name)
		return -EINVAL;

	err = dev_name_is_unique(dev);
	if(err)
		return err;

	dev->release = &dev_release;
	mutex_init(&dev->dev_lock);

	dev->file.next = NULL;
	dev->file.name = dev->name;
	vfs_add(&dev->file);
	dev_set_fops(dev, fops);

	list_add(&dev->devs, &dev_root);
	return -EOK;
}

static int _dev_set_fops(struct device *dev, struct dev_file_ops *fops)
{
	struct file *file;

	file = &dev->file;
	file->flags = _FDEV_SETUP_RW;

	if(!fops)
		return -EINVAL;

	file = &dev->file;
	file->open = fops->open;
	file->write = fops->write;
	file->read = fops->read;
	file->put = fops->put;
	file->get = fops->get;
	file->close = fops->close;
	file->flush = fops->flush;
	
	return -EOK;
}

/**
 * @brief Change the file operations of a device.
 * @param dev Device to change the file ops of.
 * @param fops New file operations.
 * @return Error code.
 * @retval <0 zero on error.
 */
int dev_set_fops(struct device *dev, struct dev_file_ops *fops)
{
	if(!dev)
		return -EINVAL;

	mutex_lock(&dev->dev_lock);
	_dev_set_fops(dev, fops);
	mutex_unlock(&dev->dev_lock);
	return -EOK;
}

/**
 * @brief Lock a device mutex.
 * @param dev Device which mutex has to be locked.
 */
void dev_lock(struct device *dev)
{
	mutex_lock(&dev->dev_lock);
}

/**
 * @brief Unlock the mutex of a device.
 * @param dev Device which mutex has to be unlocked.
 */
void dev_unlock(struct device *dev)
{
	mutex_unlock(&dev->dev_lock);
}

static struct device *dev_allocate(const char *name, struct dev_file_ops *fops)
{
	struct device *dev;

	dev = kzalloc(sizeof(*dev));

	if(!dev)
		return NULL;

	dev->name = name;
	device_initialize(dev, fops);
	return dev;
}

/**
 * @brief Lookup a device based on its name.
 * @param name Name to look for.
 * @return The device, if found.
 * @retval NULL if no device was found.
 */
struct device *dev_get_by_name(const char *name)
{
	struct device *dev;
	struct list_head *carriage;

	list_for_each(carriage, &dev_root) {
		dev = list_entry(carriage, struct device, devs);
		if(!strcmp(name, dev->name))
			return dev;
	}

	return NULL; /* no device has been found */
}

/**
 * @brief Register a platform device.
 * @param pdev Platform device to register.
 * @param fops File operations for the device.
 */
int dev_register_pdev(struct platform_device *pdev, struct dev_file_ops *fops)
{
	struct device *dev;

	if(!pdev)
		return -EINVAL;

	if(!pdev->name)
		return -EINVAL;

	dev = dev_allocate(pdev->name, fops);
	dev->pdev = pdev;
	
	return -EOK;
}

/** @} */

