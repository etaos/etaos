/*
 *  ETA/OS - USART driver core
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
#include <etaos/device.h>
#include <etaos/mutex.h>
#include <etaos/usart.h>
#include <etaos/error.h>

static inline struct usart *to_usart_dev(FILE file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);

	return dev->dev_data;
}

static int usart_write(FILE file, const void *buff, size_t len)
{
	struct usart *usart;

	usart = to_usart_dev(file);
	return usart->write(usart, buff, len);
}

static int usart_read(FILE file, void *buff, size_t len)
{
	struct usart *usart;

	usart = to_usart_dev(file);
	return usart->read(usart, buff, len);
}

static int usart_putc(int c, FILE stream)
{
	struct usart *usart;

	usart = to_usart_dev(stream);
	return usart->putc(usart, c);
}

static int usart_getc(FILE stream)
{
	struct usart *usart;

	usart = to_usart_dev(stream);
	return usart->getc(usart);
}

static struct dev_file_ops usart_fops = {
	.write = &usart_write,
	.read = &usart_read,
	.put = &usart_putc,
	.get = &usart_getc,
	.open = NULL,
};

void setup_usart_streams(struct usart *usart)
{
	FILE usart_stream;

	usart_stream = dev_to_file(&usart->dev);
	stdout = usart_stream;
	stdin = usart_stream;
	stderr = usart_stream;
}

/**
 * @brief Initialise a new USART device.
 * @param usart USART which has to be initialised.
 * @return Error code.
 * @retval -EOK on success.
 */
int usart_initialise(struct usart *usart)
{
	int err;

	err = device_initialize(&usart->dev, &usart_fops);
	if(err)
		return err;

	usart->dev.dev_data = usart;

	return -EOK;
}

