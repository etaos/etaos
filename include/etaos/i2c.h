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

/**
 * @file etaos/i2c.h
 */

/**
 * @addtogroup i2c
 */
/* @{ */

#ifndef __I2C_HEADER__
#define __I2C_HEADER__

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/device.h>
#include <etaos/stdio.h>
#include <etaos/list.h>
#include <etaos/mutex.h>

/**
 * @enum i2c_control_t
 * @brief I2C bus control functions.
 */
typedef enum {
	I2C_SET_SPEED, //!< Set the speed of an I2C bus.
} i2c_control_t;

struct i2c_client;

/**
 * @brief I2C communication structure.
 *
 * Encapsulates a buffer and its destination.
 */
struct i2c_msg {
	uint16_t dest_addr; //!< I2C chip address.
	void *buff; //!< Buffer to write to or read from.
	size_t len; //!< Length of \p buff.
	size_t idx; //!< Index in \p buff, internal use only.
	unsigned long flags; //!< Message flags.
};

/**
 * @brief I2C bus descriptor.
 * @note Only suscribed clients can write to a bus.
 */
struct i2c_bus {
	struct list_head clients; //!< List of suscribed clients.
	int timeout; //!< Transmission timeout (in between retries).
	char retries; //!< Amount of times a transmission can be done over.

	/**
	 * @brief Message transmission function pointer.
	 * @param bus Pointer to the bus handling the transmission.
	 * @param msgs Message array.
	 * @param num Length of the msgs array.
	 */	
	int (*xfer)(struct i2c_bus *bus, struct i2c_msg msgs[], int num);
	
	/**
	 * @brief Change control options of a bus.
	 * @param bus Bus to configure.
	 * @param reg Option which has to be changed.
	 * @param value Pointer to the new value.
	 */
	int (*ctrl)(struct i2c_bus *bus, unsigned long reg, void *value);

	mutex_t lock; //!< Bus lock, for mutual access.
};

/**
 * @brief I2C chip descriptor.
 *
 * An I2C client describes the layout of a chip.
 */
struct i2c_client {
	struct list_head list_entry; //!< List entry in the i2c_bus.

	struct i2c_bus *bus; //!< i2c_bus it is registered to.
	struct device dev; //!< Chip device.
	uint16_t addr; //!< I2C address.
};

/**
 * @brief I2C device information, used to create new I2C clients.
 * @see i2c_client
 */
struct i2c_device_info {
	const char *name; //!< Device name.

	uint16_t addr; //!< I2C address.
	struct dev_file_ops fops; //!< Device file operations.
	struct i2c_bus *bus; //!< i2c_bus the client should be registered to.
};

/**
 * @brief Default system bus.
 *
 * The i2c_sysbus will be used as default bus for all registering clients,
 * unless they supply their own bus in the i2c_client_info.
 */
extern struct i2c_bus *i2c_sysbus;

/**
 * @brief RX message flag.
 */
#define I2C_RD_FLAG 0

/**
 * @brief Create a new i2c_device_info structure.
 * @param name Device name.
 * @return The newly created i2c_device_info structure.
 */
static inline struct i2c_device_info *i2c_create_info(const char *name)
{
	struct i2c_device_info *info;

	info = kzalloc(sizeof(*info));

	if(!info)
		return NULL;

	info->name = name;
	return info;
}

/**
 * @brief Destroy an i2c_destroy_info structure.
 * @param info Pointer which contents should be released.
 * @note This function should be called after i2c_new_device.
 */
static inline void i2c_destroy_info(struct i2c_device_info *info)
{
	if(!info)
		return;
	else
		kfree(info);
}


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

/* @} */
#endif
