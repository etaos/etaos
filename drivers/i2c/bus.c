/*
 *  ETA/OS - I2C bus
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

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/i2c.h>
#include <etaos/list.h>

static int i2c_bus_xfer(struct i2c_bus *bus, 
		const struct i2c_msg msgs[], int len)
{
	return -EINVAL;
}

int i2c_init_bus(struct i2c_bus *bus)
{
	if(!bus)
		return -EINVAL;

	bus->xfer = &i2c_bus_xfer;
	mutex_init(&bus->lock);
	list_head_init(&bus->clients);
	return -EOK;
}

void i2c_add_client(struct i2c_bus *bus, struct i2c_client *client)
{
	if(!bus || !client)
		return;

	list_add(&client->list_entry, &bus->clients);
}

