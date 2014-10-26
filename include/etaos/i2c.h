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
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/device.h>
#include <etaos/stdio.h>
#include <etaos/list.h>
#include <etaos/mutex.h>

typedef enum {
	I2C_SET_SPEED,
} i2c_control_t;

struct i2c_client;
struct i2c_msg {
	uint16_t dest_addr;
	void *buff;
	size_t len;
	unsigned long flags;
};

struct i2c_bus {
	struct list_head clients;
	int timeout;
	char retries;
	
	int (*xfer)(struct i2c_bus *bus, struct i2c_msg msgs[], int num);
	int (*ctrl)(struct i2c_bus *bus, unsigned long reg, void *value);
	mutex_t lock;
};

struct i2c_client {
	struct list_head list_entry;

	struct i2c_bus *bus;
	struct device dev;
	uint16_t addr;
};

struct i2c_device_info {
	const char *name;

	uint16_t addr;
	struct dev_file_ops fops;
	struct i2c_bus *bus;
};

static inline struct i2c_device_info *i2c_create_info(const char *name)
{
	struct i2c_device_info *info;

	info = kzalloc(sizeof(*info));

	if(!info)
		return NULL;

	info->name = name;
	return info;
}

extern struct i2c_bus *i2c_sysbus;

#define I2C_RD_FLAG 0

extern int i2c_init_bus(struct i2c_bus *bus);
extern void i2c_add_client(struct i2c_bus *bus, struct i2c_client *client);
extern int i2c_master_send(const struct i2c_client *client, 
		const char *buf, int count);
extern int i2c_master_recv(const struct i2c_client *client, 
		char *buf, int count);
extern int i2c_bus_xfer(struct i2c_bus *bus, 
			struct i2c_msg msgs[], int len);
extern int i2c_set_bus_speed(struct i2c_bus *bus, uint32_t bps);
extern struct i2c_client *i2c_new_device(struct i2c_device_info *info);

#endif
