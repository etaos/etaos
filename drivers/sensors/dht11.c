/*
 *  ETA/OS - DHT11 sensor driver
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
 * @addtogroup dht11
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/init.h>
#include <etaos/math.h>
#include <etaos/error.h>
#include <etaos/device.h>
#include <etaos/gpio.h>
#include <etaos/preempt.h>
#include <etaos/irq.h>
#include <etaos/tick.h>
#include <etaos/mem.h>
#include <etaos/delay.h>
#include <etaos/platform.h>
#include <etaos/dht11.h>

struct dht11 {
	struct device dev;
	struct gpio_pin *pin;
	tick_t last_read;
	bool last_read_ok;
	unsigned long cycles;
	uint8_t data[5];
	bool read_temp;
	dht_mode_t mode;
};

#define LOW false
#define HIGH true

static inline struct dht11 *to_dht_chip(struct file *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	return dev->dev_data;
}

static inline unsigned long dht_cycles_per_us(unsigned long us)
{
	return us * (CONFIG_FCPU / 1000000UL);
}

static uint32_t dht_expect(struct dht11 *dht, bool level)
{
	uint32_t count = 1;

	while(raw_gpio_read_pin(dht->pin) == level) {
		if(count++ >= dht->cycles+1)
			return 0;
	}

	return count;
}

static void dht_delay(int ms)
{
#ifdef CONFIG_SCHED
	sleep(ms);
#else
	delay(ms);
#endif
}

/* Implementation if DHT11 weirdo 1-wire protocol */
static bool raw_dht11_read(struct dht11 *dht)
{
	tick_t current = sys_tick;
	uint32_t *cyclesdata;
	unsigned long flags;
	uint32_t lowc, highc;
	int i;
	static uint8_t data[5];

	cyclesdata = kzalloc(80 * sizeof(uint32_t));
	dht->last_read = current;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	preempt_disable();
	__raw_gpio_pin_write(dht->pin, HIGH);
	dht_delay(250);

	gpio_direction_output(dht->pin, LOW);
	delay(20);

	irq_save_and_disable(&flags);
	__raw_gpio_pin_write(dht->pin, HIGH);
	delay_us(40);

	gpio_direction_input(dht->pin);
	__raw_gpio_pin_write(dht->pin, HIGH);
	delay_us(10);

	if(dht_expect(dht, LOW) == 0) {
		irq_restore(&flags);
		preempt_enable();
		kfree(cyclesdata);
		return dht->last_read_ok;
	}

	if(dht_expect(dht, HIGH) == 0) {
		irq_restore(&flags);
		preempt_enable();
		kfree(cyclesdata);
		return dht->last_read_ok;
	}

	/* Read 40 bits of data sent by the sensor */
	for(i = 0; i < 80; i += 2) {
		cyclesdata[i] = dht_expect(dht, LOW);
		cyclesdata[i+1] = dht_expect(dht, HIGH);
	}
	irq_restore(&flags);
	preempt_enable();

	for(i = 0; i < 40; i++) {
		lowc = cyclesdata[2*i];
		highc = cyclesdata[2*i + 1];

		if(!lowc || !highc) {
			preempt_enable();
			kfree(cyclesdata);
			dht->last_read_ok = false;
			return dht->last_read_ok;
		}

		data[i/8] <<= 1;

		if(highc >= lowc)
			data[i/8] |= 1;
	}

	/* PARITY check */
	kfree(cyclesdata);

	if (data[4] == ((data[0] + data[1] +
		data[2] + data[3]) & 0xFF)) {
		dht->last_read_ok = true;

		dht->data[0] = data[0];
		dht->data[1] = data[1];
		dht->data[2] = data[2];
		dht->data[3] = data[3];
		dht->data[4] = data[4];
	} else {
		dht->last_read_ok = false;
	}

	return dht->last_read_ok;
}

/**
 * @brief Open the DHT11 device file.
 * @param file File to open.
 * @return Error code
 */
static int dht_open(struct file *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	dev_lock(dev);
	return -EOK;
}

/**
 * @brief Close the DHT11 device file.
 * @param file File to close.
 * @return Error code
 */
static int dht_close(struct file *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	dev_unlock(dev);
	return -EOK;
}

/**
 * @brief Change DHT11 control options.
 * @param file Device file.
 * @param reg Control register.
 * @param buf Argument to \p reg.
 * @return An error code, -EOK on success.
 */
static int dht_ioctl(struct file *file, unsigned long reg, void *buf)
{
	int rc = -EINVAL;
	struct dht11 *dht = to_dht_chip(file);

	switch(reg) {
	case DHT_SET_PIN:
		dht->pin = buf;
		gpio_pin_request(dht->pin);
		gpio_direction_input(dht->pin);
		__raw_gpio_pin_write(dht->pin, HIGH);
		rc = -EOK;
		break;

	case DHT_RELEASE_PIN:
		if(!dht->pin)
			return rc;

		gpio_direction_output(dht->pin, LOW);
		gpio_pin_release(dht->pin);
		rc = -EOK;
		break;

	case DHT_MEASURE_HUMIDITY:
		dht->read_temp = false;
		rc = -EOK;
		break;

	case DHT_MEASURE_TEMPERATURE:
		dht->read_temp = true;
		rc = -EOK;
		break;

	case DHT_MODE_DHT11:
		dht->mode = DHT11;
		rc = -EOK;
		break;

	case DHT_MODE_DHT21:
	case DHT_MODE_DHT22:
		dht->mode = DHT22;
		rc = -EOK;
		break;

	default:
		break;
	}

	return rc;
}

#define DHT22_NEG_BIT 7
#define DHT_TIMEOUT_PERIOD 2000LL

/**
 * @brief Read from the DHT11 sensor.
 * @param file Device file.
 * @param buf Data buffer.
 * @param length Length of \p buf.
 * @note The length of buf must be `sizeof(float)`.
 * @return The number of bytes read or an error code.
 */
static int dht_read(struct file *file, void *buf, size_t length)
{
	float f = NAN;
	struct dht11 *chip = to_dht_chip(file);

	if(length != sizeof(f) || !buf)
		return -EINVAL;

	if(time_after(sys_tick, chip->last_read + DHT_TIMEOUT_PERIOD)) {
		if(!raw_dht11_read(chip)) {
			*((float*)buf) = NAN;
			return -EINVAL;
		}
	}

	if(!chip->last_read_ok) {
		*((float*)buf) = NAN;
		return -EINVAL;
	}

	switch(chip->mode) {
	case DHT11:
		if(chip->read_temp)
			f = chip->data[2];
		else
			f = chip->data[0];
		break;

	case DHT21:
	case DHT22:
		if(chip->read_temp) {
			f = (float)(chip->data[2] & 0x7F);
			f *= 256.0;
			f += chip->data[3];
			f *= 0.1;
			if(test_bit(DHT22_NEG_BIT, &chip->data[2]))
				f *= -1.0;
		} else {
			/* read humidity */
			f = chip->data[0];
			f *= 256.0;
			f += chip->data[1];
			f *= 0.1;
		}
		break;
	}

	*((float*)buf) = f;
	return sizeof(f);
}

static struct dev_file_ops dht_ops = {
	.read = &dht_read,
	.open = &dht_open,
	.close = &dht_close,
	.ioctl = &dht_ioctl,
};

static struct dht11 dhtchip;

static void __used dht_init(void)
{
	dhtchip.last_read = 0LL;
	dhtchip.mode = DHT11;
	dhtchip.last_read_ok = false;
	dhtchip.read_temp = false;
	dhtchip.cycles = dht_cycles_per_us(1000);
	dhtchip.dev.name = "dht11";
	device_initialize(&dhtchip.dev, &dht_ops);
	dhtchip.dev.dev_data = &dhtchip;
}

module_init(dht_init);

/** @} */
