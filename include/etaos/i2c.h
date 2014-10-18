/*
 *  ETA/OS - I2C header
 *  Copyright (C) 2014   Michel Megens
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

#ifndef __I2C_HEADER__
#define __I2C_HEADER__

#include <etaos/kernel.h>
#include <etaos/device.h>
#include <etaos/stdio.h>
#include <etaos/list.h>
#include <etaos/mutex.h>

struct i2c_client;

struct i2c_msg {
	uint16_t dest_addr;
	void *buff;
	size_t len;
};

struct i2c_bus {
	struct list_head clients;
	int timeout;
	
	int (*xfer)(struct i2c_bus *bus, const struct i2c_msg msgs[], int num);
	mutex_t lock;
};

struct i2c_client {
	struct list_head *list_entry;

	struct i2c_bus *bus;
	struct device dev;
	uint16_t addr;
};

extern int i2c_init_bus(struct i2c_bus *bus);

#endif
