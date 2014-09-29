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

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/mutex.h>

struct platform_device {
	const char *name;
	void *io_base;
};

struct device {
	const char *name;
	struct list_head devs;
	FILE file;

	mutex_t dev_lock;
	struct platform_device *pdev;
	void *dev_data;

	void (*release)(struct device *dev);
};

struct dev_file_ops {
	int (*close)(struct file*);
	int (*read)(struct file*, void*, size_t);
	int (*write)(struct file*, void*, size_t);
	int (*flush)(struct file*);
	int (*put)(int c, struct file*);
	int (*get)(int c, struct file*);
};

extern int device_create(struct device *dev, struct dev_file_ops *fops);
extern int pdev_add(struct platform_device *pdev, struct dev_file_ops *fops);
extern void dev_core_init();
extern int dev_set_fops(struct device *dev, struct dev_file_ops *fops);
extern struct device *dev_get_by_name(const char *name);

static inline void *pdev_get_iobase(struct platform_device *pdev)
{
	if(pdev)
		return pdev->io_base;

	return NULL;
}

static inline void *dev_get_iobase(struct device *dev)
{
	if(dev)
		return pdev_get_iobase(dev->pdev);

	return NULL;
}

#endif
