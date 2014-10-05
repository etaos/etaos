/*
 *  ETA/OS - USART driver header
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

#ifndef __USART_H__
#define __USART_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/device.h>
#include <etaos/mutex.h>

struct usart {
	mutex_t bus_lock;
	int timeout;

	struct device dev;
	int (*write)(struct usart *uart, const void *tx,
			size_t txlen);
	int (*putc)(struct usart*, int);
	int (*getc)(struct usart*);
	int (*read)(struct usart *uart, void *rx, size_t len);
};

extern int usart_initialise(struct usart *usart);

#endif
