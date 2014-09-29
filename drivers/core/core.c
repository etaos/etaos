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

static struct list_head dev_root;

static int _dev_set_fops(struct device *dev, struct dev_file_ops *fops);

void dev_core_init(void)
{
	list_head_init(&dev_root);
}

static void device_release(struct device *dev)
{
	FILE file;

	if(!dev)
		return;

	file = dev->file;
	list_del(&dev->devs);
	kfree(file);
	kfree(dev);
}

int device_create(struct device *dev, struct dev_file_ops *fops)
{
	if(!dev)
		return -EINVAL;

	if(!dev->file) {
		dev->file = kmalloc(sizeof(struct file));
		if(!dev->file)
			return -ENOMEM;
	}

	_dev_set_fops(dev, fops);
	list_add(&dev->devs, &dev_root);
	return 0;
}

static int _dev_set_fops(struct device *dev, struct dev_file_ops *fops)
{
	struct file *file;

	file = dev->file;

	file->write = fops->write;
	file->read = fops->read;
	file->put = fops->put;
	file->get = fops->get;
	file->close = fops->close;
	file->flush = fops->flush;
	
	return -EOK;
}

int dev_set_fops(struct device *dev, struct dev_file_ops *fops)
{
	if(!dev)
		return -EINVAL;

	mutex_lock(&dev->dev_lock);
	_dev_set_fops(dev, fops);
	mutex_unlock(&dev->dev_lock);
	return -EOK;
}

static struct device *dev_allocate(struct dev_file_ops *fops)
{
	struct device *dev;
	FILE file;

	dev = kmalloc(sizeof(*dev));
	file = kmalloc(sizeof(*file));

	if(!dev || !file)
		return NULL;

	dev->release = &device_release;
	mutex_init(&dev->dev_lock);

	dev->file = file;
	dev_set_fops(dev, fops);

	return dev;
}

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

int pdev_add(struct platform_device *pdev, struct dev_file_ops *fops)
{
	struct device *dev;

	if(!pdev)
		return -EINVAL;

	dev = dev_allocate(fops);
	dev->pdev = pdev;
	dev->name = pdev->name;
	list_add(&dev->devs, &dev_root);

	return -EOK;
}

