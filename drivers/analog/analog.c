/*
 *  ETA/OS - Analog IO driver
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
 * @addtogroup analog
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/device.h>
#include <etaos/analog.h>
#include <etaos/init.h>
#include <etaos/mem.h>

/**
 * @brief Reference to the default analog chip.
 */
struct analog_chip *analog_syschip;

static inline struct analog_chip *to_chip(struct vfile *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	return dev->dev_data;
}

/**
 * @brief Read from the ADC.
 * @param file Device file used to access the ADC.
 * @return The value read from the ADC.
 * @see gets fgets
 */
static int analog_get(struct vfile *file)
{
	struct analog_chip *chip;

	chip = to_chip(file);
	return chip->get(chip->selected_pin);
}

/**
 * @brief Read from the ADC.
 * @param pin Analog pin to read from.
 * @return The value read from the ADC.
 * @see analog_get
 * @note This is an API alternative to analog_get (using the device tree).
 */
int analog_read(struct analog_pin *pin)
{
	struct analog_chip *chip;

	chip = pin->chip;
	return chip->get(pin);
}

/**
 * @brief Configure the ADC.
 * @param file Device file used to access the ADC.
 * @param reg Configuration register.
 * @param arg Argument to \p reg.
 * @return Error code.
 * @see ioctl ANALOG_SELECT_PIN
 */
static int analog_ctl(struct vfile *file, unsigned long reg, void *arg)
{
	struct analog_chip *chip;
	int pin;

	chip = to_chip(file);

	switch(reg) {
	case ANALOG_SELECT_PIN:
		pin = *(int*)arg;

		if(pin >= chip->num)
			return -EINVAL;

		chip->selected_pin = &chip->pins[pin];
		break;
	default:
		return chip->ioctl(chip, reg, arg);
	}

	return -EOK;
}

/**
 * @brief Open the ADC device.
 * @param file Device file used to access the ADC.
 * @note Acquires the ADC device lock.
 * @return An error code.
 */
static int analog_open(struct vfile *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	dev_lock(dev);
	return -EOK;
}

/**
 * @brief Close the ADC device.
 * @param file Device file used to access the ADC.
 * @note Releases the ADC device lock.
 * @return An error code.
 */
static int analog_close(struct vfile *file)
{
	struct device *dev;

	dev = container_of(file, struct device, file);
	dev_unlock(dev);
	return -EOK;
}

static struct dev_file_ops analog_fops = {
	.open = analog_open,
	.close = analog_close,
	.ioctl = &analog_ctl,
	.get = &analog_get,
};

/**
 * @brief Initialise a new ADC chip.
 * @param chip ADC chip to initialise.
 * @note This function also initialises the pin array; struct analog_chip::num
 *       should be initialised prior to calling this function.
 * @return An error code.
 */
int analog_chip_init(struct analog_chip *chip)
{
	struct analog_pin *pin_ary;
	int i = 0;

	if(!chip->num)
		return -EINVAL;

	device_initialize(&chip->dev, &analog_fops);
	chip->dev.dev_data = chip;

	pin_ary = kzalloc(sizeof(*pin_ary) * chip->num);
	if(!pin_ary)
		return -ENOMEM;

	for(; i < chip->num; i++) {
		pin_ary[i].chip = chip;
		pin_ary[i].num = i;
	}

	chip->pins = pin_ary;
	return -EOK;
}

static void __used analog_init(void)
{
}

subsys_init(analog_init);

/** @} */

