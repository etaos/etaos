/*
 *  ETA/OS - FLASH
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
 * @addtogroup flash
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/device.h>
#include <etaos/flash.h>
#include <etaos/init.h>

static inline struct device *file_to_dev(struct file *file)
{
	return container_of(file, struct device, file);
}

static inline struct flash_chip *file_to_flash(struct file *file)
{
	struct device *dev;

	dev = file_to_dev(file);
	return (struct flash_chip*)dev->dev_data;
}

static int flash_open(struct file *flash)
{
	struct device *dev;

	dev = file_to_dev(flash);
	dev_lock(dev);

	return -EOK;
}

static int flash_close(struct file *flash)
{
	struct device *dev;

	dev = file_to_dev(flash);
	dev_unlock(dev);

	return -EOK;
}

static int flash_read(struct file *stream, void *buff, size_t num)
{
	struct flash_chip *flash;
	int rc;

	if(!stream)
		return -EINVAL;

	flash = file_to_flash(stream);
	rc = flash->read(flash, stream->index, buff, num);

	if(rc >= 0)
		stream->index += num;

	return rc;
}

static int flash_getc(struct file *stream)
{
	struct flash_chip *flash;
	int rc;

	if(!stream)
		return -EINVAL;

	flash = file_to_flash(stream);
	rc = flash->getc(flash, stream->index);

	if(rc >= 0)
		stream->index++;

	return rc;
}

/**
 * @brief Flash file operations.
 */
struct dev_file_ops flash_ops = {
	.read = &flash_read,
	.get = &flash_getc,
	.open = &flash_open,
	.close = &flash_close,
};

/**
 * @brief Initialise a new flash chip.
 * @param flash Flash chip to initialise.
 * @see device_initialize
 */
void flash_chip_init(struct flash_chip *flash)
{
	if(!flash)
		return;

	device_initialize(&flash->dev, &flash_ops);
	flash->dev.dev_data = flash;
}

static __used void flash_init(void)
{
}

subsys_init(flash_init);

/** @} */
