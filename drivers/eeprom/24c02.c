/*
 *  ETA/OS - EEPROM header
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

/**
 * @addtogroup ee-24c02
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/i2c.h>
#include <etaos/eeprom.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/device.h>

#include <etaos/eeprom/24c02.h>

#define BASE_SLA_24C02 0xA0
#define SCL_FRQ_24C02 100000UL
#define EE_SYNC 10

static int __eeprom_24c02_read_byte(struct eeprom *ee)
{
	unsigned char store;

	eeprom_24c02_read_byte(ee->rd_idx, &store);
	return store;
}

static int __eeprom_24c02_write_byte(struct eeprom *ee, int c)
{
	return eeprom_24c02_write_byte(ee->wr_idx, (unsigned char)c);
}

static struct eeprom ee_chip = {
	.name = "24C02",
	.write_byte = &__eeprom_24c02_write_byte,
	.read_byte = &__eeprom_24c02_read_byte,
};

/**
 * @brief Write a single byte to a 24C02 EEPROM chip.
 * @param addr EEPROM address to write to.
 * @param data Data byte to write to \p addr.
 * @return Error code.
 */
int eeprom_24c02_write_byte(unsigned char addr, unsigned char data)
{
	int rc;
	struct i2c_client *client = ee_chip.priv;
	unsigned char tx[] = { addr, data };

	dev_sync_lock(&client->dev, EE_SYNC);
	rc = i2c_master_send(ee_chip.priv, (void*)tx, 2);
	dev_sync_unlock(&client->dev, EE_SYNC);

	return (rc == 2) ? -EOK : rc;
}

#define MSG_TX 0
#define MSG_RX 1
/**
 * @brief Read a byte from a 24C02 EEPROM chip.
 * @param addr EEPROM address to read from.
 * @param storage Pointer to an address to store the byte read from EEPROM.
 * @return An error code.
 */
int eeprom_24c02_read_byte(unsigned char addr, unsigned char *storage)
{
	int rc;
	unsigned char rx;
	struct i2c_client *client = ee_chip.priv;
	struct i2c_msg *msgs;

	msgs = kzalloc(sizeof(*msgs)*2);
	msgs[MSG_TX].dest_addr = client->addr;
	msgs[MSG_TX].len = 1;
	msgs[MSG_TX].buff = &addr;

	msgs[MSG_RX].dest_addr = client->addr;
	msgs[MSG_RX].len = 1;
	msgs[MSG_RX].buff = &rx;
	set_bit(I2C_RD_FLAG, &msgs[MSG_RX].flags);

	dev_sync_lock(&client->dev, EE_SYNC);
	rc = i2c_bus_xfer(client->bus, msgs, 2);
	dev_sync_unlock(&client->dev, EE_SYNC);

	*storage = rx;
	kfree(msgs);

	return (rc == 2) ? -EOK : rc;
}

/**
 * @brief Initialise an 24C02 EEPROM chip.
 */
void eeprom_init_24c02(void)
{
	struct i2c_device_info *info;
	struct i2c_client *client;

	info = i2c_create_info("24C02");
	info->addr = BASE_SLA_24C02;
	ee_chip.priv = i2c_new_device(info);
	client = ee_chip.priv;
	kfree(info);

	eeprom_chip_init(&ee_chip, &client->dev);

	return;
}

device_init(eeprom_init_24c02);

/** @} */

