/*
 *  ETA/OS - BMP085 driver
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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

/**
 * @addtogroup bmp085
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdlib.h>
#include <etaos/string.h>
#include <etaos/types.h>
#include <etaos/i2c.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/init.h>
#include <etaos/device.h>
#include <etaos/thread.h>
#include <etaos/delay.h>

#define BMP_ADDR 0xEE

#define BMP_LOWPOWER    0
#define BMP_STANDARD    1
#define BMP_HIRES       2
#define BMP_ULTRA_HIRES 3

struct bmp085 {
	const char *name;
	struct i2c_client *client;
	bool needs_calibration;
	uint8_t oversampling;

	int16_t ac1, ac2, ac3;
	uint16_t ac4, ac5, ac6;
	int16_t b1, b2;
	int16_t mb, mc, md;
};

static struct bmp085 bmpchip = {
	.name = "bmp085",
	.needs_calibration = true,
};

#define MSG_TX 0
#define MSG_RX 1

static int raw_bmp_read16(struct i2c_client *client, uint8_t reg, void *_buf, int len)
{
	uint16_t *buf;
	struct i2c_msg *msgs;
	int rc;

	msgs = kzalloc(sizeof(*msgs)*2);
	if(!msgs)
		return -ENOMEM;

	msgs[MSG_TX].dest_addr = client->addr;
	msgs[MSG_TX].len = 1;
	msgs[MSG_TX].buff = &reg;

	msgs[MSG_RX].dest_addr = client->addr;
	msgs[MSG_RX].len = len;
	msgs[MSG_RX].buff = _buf;
	set_bit(I2C_RD_FLAG, &msgs[MSG_RX].flags);
	rc = i2c_bus_xfer(client->bus, msgs, 2);
	kfree(msgs);

	if(len == 1)
		return rc == 2 ? len : rc;

	if(rc == 2 && len == 2) {
		buf = _buf;
		*buf = (*buf << 8) + (*buf >> 8);
		return len;
	}

	return rc;
}

static int raw_bmp_write8(struct i2c_client *client, uint8_t reg, uint8_t data)
{
	int rc;
	uint8_t _data[] = {reg, data};

	rc = i2c_master_send(client, (void*)_data, 2);
	return rc == 2 ? rc : -EINVAL;
}

#define BMP_CHECK_REG 0xD0
#define BMP_CHECK_VALUE 0x55

#define BMP_A1_REG 0xAA
#define BMP_A2_REG 0xAC
#define BMP_A3_REG 0xAE
#define BMP_A4_REG 0xB0
#define BMP_A5_REG 0xB2
#define BMP_A6_REG 0xB4
#define BMP_B1_REG 0xB6
#define BMP_B2_REG 0xB8
#define BMP_MB_REG 0xBA
#define BMP_MC_REG 0xBC
#define BMP_MD_REG 0xBE

static int bmp085_calibrate(struct bmp085 *bmp)
{
	uint8_t check;
	int rc;

	rc = raw_bmp_read16(bmp->client, BMP_CHECK_REG, &check, sizeof(check));
	if(rc != sizeof(check) || check != BMP_CHECK_VALUE)
		return -EINVAL;

	raw_bmp_read16(bmp->client, BMP_A1_REG, &bmp->ac1, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_A2_REG, &bmp->ac2, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_A3_REG, &bmp->ac3, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_A4_REG, &bmp->ac4, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_A5_REG, &bmp->ac5, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_A6_REG, &bmp->ac6, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_A1_REG, &bmp->ac1, sizeof(uint16_t));

	raw_bmp_read16(bmp->client, BMP_B1_REG, &bmp->b1, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_B2_REG, &bmp->b2, sizeof(uint16_t));

	raw_bmp_read16(bmp->client, BMP_MB_REG, &bmp->mb, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_MC_REG, &bmp->mc, sizeof(uint16_t));
	raw_bmp_read16(bmp->client, BMP_MD_REG, &bmp->md, sizeof(uint16_t));

	bmp->needs_calibration = false;

	return -EOK;
}

static void bmp_delay(int ms)
{
#ifdef CONFIG_SCHED
	sleep(ms);
#else
	delay(ms);
#endif
}

#define BMP_CONTROL 0xF4
#define BMP_READ_TEMPERATURE_CMD 0x2E
#define BMP_READ_PRESSURE_CMD 0x34
#define BMP_READ_TEMPDATA 0xF6
#define BMP_READ_PRESDATA 0xF6

static uint16_t bmp_read_raw_temperature(struct bmp085 *bmp)
{
	uint16_t ut;

	raw_bmp_write8(bmp->client, BMP_CONTROL, BMP_READ_TEMPERATURE_CMD);
	delay(5);
	raw_bmp_read16(bmp->client, BMP_READ_TEMPDATA, &ut, sizeof(ut));
	return ut;
}

static uint32_t bmp_read_raw_pressure(struct bmp085 *bmp)
{
	uint32_t raw = 0UL;
	int msdelay;
	uint16_t raw16;
	uint8_t raw8;

	raw_bmp_write8(bmp->client, BMP_CONTROL, BMP_READ_PRESSURE_CMD + (bmp->oversampling << 6));

	switch(bmp->oversampling) {
	case BMP_LOWPOWER:
		msdelay = 5;
		break;

	case BMP_STANDARD:
		msdelay = 8;
		break;

	case BMP_HIRES:
		msdelay = 14;
		break;

	case BMP_ULTRA_HIRES:
		msdelay = 26;
		break;

	default:
		return 0;
	}

	bmp_delay(msdelay);

	raw_bmp_read16(bmp->client, BMP_READ_PRESDATA, &raw16, sizeof(raw16));
	raw_bmp_read16(bmp->client, BMP_READ_PRESDATA+sizeof(raw16), &raw8, sizeof(raw8));

	raw = raw16;
	raw <<= 8;
	raw |= raw8;
	raw >>= (8 - bmp->oversampling);

	return raw;
}

static inline int32_t bmp_compute_b5(struct bmp085 *bmp, int32_t ut)
{
	int32_t x1, x2;

	x1 = (ut - (int32_t)bmp->ac6) * ((int32_t)bmp->ac5) >> 15;
	x2 = ((int32_t)bmp->mc << 11) / (x1 + (int32_t)bmp->md);
	return x1 + x2;
}

static int32_t bmp_calc_pressure(struct bmp085 * bmp)
{
	int32_t up, ut, b5, b6, x1, x2, x3, p, b3;
	uint32_t b4, b7;

	up = bmp_read_raw_pressure(bmp);
	ut = bmp_read_raw_temperature(bmp);

	b5 = bmp_compute_b5(bmp, ut);

	//calculate true pressure
	b6 = b5 - 4000;
	x1 = ((int32_t)bmp->b2 * ((b6 * b6) >> 12)) >> 11;
	x2 = ((int32_t)bmp->ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = ((( (int32_t)bmp->ac1 * 4 + x3) << bmp->oversampling) + 2) / 4;

	x1 = ((int32_t)bmp->ac3 * b6) >> 13;
	x2 = ((int32_t)bmp->b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = ((uint32_t)bmp->ac4 * (uint32_t) (x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (uint32_t)(50000UL >> bmp->oversampling);
	p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;

	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p = p + ((x1 + x2 + 3791) >> 4);
	return p;
}

static inline struct bmp085 *to_bmp_chip(struct file *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	return dev->dev_data;
}

/**
 * @brief Open the BMP085 device file.
 * @param file File to open.
 * @return Error code
 */
static int bmp_open(struct file *file)
{
	struct bmp085 *bmp;
	struct device *dev;
	int rc = -EOK;

	dev = container_of(file, struct device, file);
	dev_lock(dev);
	bmp = dev->dev_data;

	if(bmp->needs_calibration)
		rc = bmp085_calibrate(bmp);

	if(rc)
		dev_unlock(dev);

	return rc;
}

/**
 * @brief Close the BMP085 device file.
 * @param file File to close.
 * @return Error code
 */
static int bmp_close(struct file *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	dev_unlock(dev);
	return -EOK;
}

/**
 * @brief Read from the BMP085 sensor.
 * @param file Device file.
 * @param buf Data buffer.
 * @param len Length of \p buf.
 * @note The length of buf must be `sizeof(int32_t)`.
 * @return The number of bytes read or an error code.
 */
static int bmp_read(struct file *file, void *buf, size_t len)
{
	int32_t *_buf;
	struct bmp085 *bmp;

	if(len != sizeof(int32_t) || !buf)
		return -EINVAL;

	bmp = to_bmp_chip(file);
	_buf = buf;
	*_buf = bmp_calc_pressure(bmp);

	return (_buf != 0) ? len : -EINVAL;
}

static struct dev_file_ops bmpfops = {
	.open = &bmp_open,
	.close = &bmp_close,
	.read = &bmp_read,
};

void bmp085_init(void)
{
	struct i2c_device_info *info;
	struct i2c_client *client;

	bmpchip.oversampling = BMP_ULTRA_HIRES;

	info = i2c_create_info(bmpchip.name);
	info->addr = BMP_ADDR;
	bmpchip.client = i2c_new_device(info);
	client = bmpchip.client;

	client->dev.name = bmpchip.name;
	device_initialize(&client->dev, &bmpfops);
	client->dev.dev_data = &bmpchip;
	kfree(info);

	return;
}

device_init(bmp085_init);

/** @} */
