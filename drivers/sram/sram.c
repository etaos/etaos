/*
 *  ETA/OS - SRAM core driver
 *  Copyright (C) 2015   Michel Megens <dev@michelmegens.net>
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
 * @file etaos/sram.h
 * @addtogroup sram
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/sram.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/device.h>
#include <etaos/error.h>
#include <etaos/mutex.h>

/**
 * @brief Convert a file pointer to a SRAM structure.
 * @param file struct vfile * pointer to convert.
 * @return SRAM data structure.
 */
static inline struct sram *to_sram_chip(struct vfile * file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	return dev->dev_data;
}

/**
 * @brief Write function pointer.
 * @param stream File SRAM chip descriptor.
 * @param buf Buffer which has to be written.
 * @param len Length of \p buf.
 * @return Error code or the number of bytes written.
 */
static int sram_write(struct vfile * stream, const void *buf, size_t len)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	if(!ram->write)
		return -EINVAL;

	rc = ram->write(ram, buf, len);
	stream->index += len;

	return rc;
}

/**
 * @brief Read function pointer.
 * @param stream File SRAM chip descriptor.
 * @param buf Buffer to read data into.
 * @param len Length of \p buf.
 * @return Error code or number bytes read.
 */
static int sram_read(struct vfile * stream, void *buf, size_t len)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	if(!ram->read)
		return -EINVAL;

	rc = ram->read(ram, buf, len);
	stream->index += len;

	return rc;
}

/**
 * @brief Write a single byte to the SRAM chip memory.
 * @param c Byte which has to be written.
 * @param stream File SRAM chip descriptor.
 * @return Error code or the number of bytes written.
 */
static int sram_put(int c, struct vfile * stream)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	if(!ram->write_byte)
		return -EINVAL;

	rc = ram->write_byte(ram, c);
	stream->index += 1;

	return rc;
}

/**
 * @brief Read a single byte from the SRAM chip memory.
 * @param stream File SRAM chip descriptor.
 * @return Error code or the number of bytes read.
 */
static int sram_get(struct vfile * stream)
{
	int rc;
	struct sram *ram;

	if(!stream)
		return -EINVAL;

	ram = to_sram_chip(stream);
	if(!ram->read_byte)
		return -EINVAL;

	rc = ram->read_byte(ram);
	stream->index += 1;

	return rc;
}

static int sram_open(struct vfile *file)
{
	struct device *dev = container_of(file, struct device, file);

	dev_lock(dev);
	return -EOK;
}

static int sram_close(struct vfile *file)
{
	struct device *dev = container_of(file, struct device, file);

	dev_unlock(dev);
	return -EOK;
}

static struct dev_file_ops sram_ops = {
	.write = &sram_write,
	.read = &sram_read,
	.put = &sram_put,
	.get = &sram_get,
	.open = &sram_open,
	.close = &sram_close,
};

/**
 * @brief Initialise a new SRAM chip data structure.
 * @param ram SRAM structure which has to be initialised.
 * @param dev Supporting device structure.
 */
void sram_chip_init(struct sram *ram, struct device *dev)
{
	if(dev) {
		dev->name = ram->name;
		device_initialize(dev, &sram_ops);
		dev->dev_data = ram;
		ram->file = &dev->file;
	}
}

static void __used sram_init(void)
{
}

/** @} */

subsys_init(sram_init);

