/*
 *  ETA/OS - USART driver core
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/device.h>
#include <etaos/mutex.h>
#include <etaos/usart.h>
#include <etaos/error.h>
#include <etaos/init.h>

static inline struct usart *to_usart_dev(struct file * file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);

	return dev->dev_data;
}

static int usart_write(struct file * file, const void *buff, size_t len)
{
	struct usart *usart;
	int rv;

	usart = to_usart_dev(file);
	mutex_lock(&usart->tx);
	rv = usart->write(usart, buff, len);
	mutex_unlock(&usart->tx);

	return rv;
}

static int usart_read(struct file * file, void *buff, size_t len)
{
	struct usart *usart;
	int rv;

	usart = to_usart_dev(file);
	mutex_lock(&usart->rx);
	rv = usart->read(usart, buff, len);
	mutex_unlock(&usart->rx);
	return rv;
}

static int usart_putc(int c, struct file * stream)
{
	struct usart *usart;
	int rv;

	usart = to_usart_dev(stream);
	mutex_lock(&usart->tx);
	rv = usart->putc(usart, c);
	mutex_unlock(&usart->tx);

	return rv;
}

static int usart_getc(struct file * stream)
{
	struct usart *usart;
	int rv;

	usart = to_usart_dev(stream);
	mutex_lock(&usart->rx);
	rv = usart->getc(usart);
	mutex_unlock(&usart->rx);

	return rv;
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
	struct file * usart_stream;

	usart_stream = dev_to_file(&usart->dev);
	sysctl(SYS_SET_STDOUT, usart_stream);
	sysctl(SYS_SET_STDERR, usart_stream);
	sysctl(SYS_SET_STDIN, usart_stream);
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
	mutex_init(&usart->rx);
	mutex_init(&usart->tx);
	if(err)
		return err;

	usart->dev.dev_data = usart;

	return -EOK;
}

static void __used usart_core_init(void)
{
}

subsys_init(usart_core_init);

