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
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/tick.h>

/**
 * @addtogroup i2c
 */
/* @{ */

struct i2c_bus *i2c_sysbus;

/**
 * @brief Transfer a given amount of I2C messages.
 * @param bus I2C bus to use for the transmission.
 * @param msgs Array of messages to transmit from or receive data into.
 * @param len Length of the message array.
 * @return Error code.
 * @retval -EOK on success.
 * @retval -EINVAL on error.
 */
static int __i2c_transfer(struct i2c_bus *bus,
		struct i2c_msg msgs[], int len)
{
	int ret;
	char retries;
	int64_t orig_tick;

	orig_tick = sys_tick;
	for(retries = 0, ret = 0; retries < bus->retries; retries++) {
		ret = bus->xfer(bus, msgs, len);

		if(ret != -EAGAIN)
			break;
		if(time_after(orig_tick, orig_tick + bus->timeout))
			break;
	}

	return ret;
}

/**
 * @brief Transfer a given amount of I2C messages.
 * @param bus I2C bus to use for the transmission.
 * @param msgs Array of messages to transmit from or receive data into.
 * @param len Length of the message array.
 * @return Amount of messages transferred or an error code.
 * @retval -EINVAL on error.
 */
int i2c_bus_xfer(struct i2c_bus *bus, 
			struct i2c_msg msgs[], int len)
{
	int ret;

	mutex_lock(&bus->lock);
	ret = __i2c_transfer(bus, msgs, len);
	mutex_unlock(&bus->lock);

	return (ret == -EOK) ? len : ret;
}

/**
 * @brief Set the I2C bus operating speed.
 * @param bus I2C bus to set the speed for.
 * @param bps New speed in bits per second (or Hertz).
 * @return An error code.
 * @retval -EOK on success.
 * @retval -EINVAL on error.
 */
int i2c_set_bus_speed(struct i2c_bus *bus, uint32_t bps)
{
	int ret;

	if(!bus || !bps)
		return -EINVAL;

	mutex_lock(&bus->lock);
	ret = bus->ctrl(bus, I2C_SET_SPEED, &bps);
	mutex_unlock(&bus->lock);

	return ret;
}

/**
 * @brief Initialise a new I2C bus.
 * @param bus I2C bus which has to be initialised.
 * @return Error code.
 * @retval -EINVAL if \p bus is invalid.
 * @retval -EOK if \p bus is succesfully initialised.
 */
int i2c_init_bus(struct i2c_bus *bus)
{
	if(!bus)
		return -EINVAL;

	mutex_init(&bus->lock);
	list_head_init(&bus->clients);
	return -EOK;
}

/**
 * @brief Add a new client to a bus.
 * @param bus Bus to add \p client to.
 * @param client I2C client to add to \p bus.
 */
void i2c_add_client(struct i2c_bus *bus, struct i2c_client *client)
{
	if(!bus || !client)
		return;

	list_add(&client->list_entry, &bus->clients);
	client->bus = bus;
}

/* @} */

