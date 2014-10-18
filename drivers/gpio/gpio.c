/*
 *  ETA/OS - GPIO driver
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/spinlock.h>
#include <etaos/gpio.h>
#include <etaos/bitops.h>

int gpio_chip_init(struct gpio_chip *chip, uint16_t pins)
{
	int err;

	chip->pins = kmalloc(sizeof(*chip->pins) * pins);
	chip->num = pins;
	if(!chip->pins)
		return -ENOMEM;
	else
		err = -EOK;

	spin_lock_init(&chip->lock);

	return err;
}

void gpio_pin_init(struct gpio_pin *pin, unsigned long flags)
{
	if(!pin)
		return;

	clear_bit(GPIO_REQUESTED, &flags);
	pin->flags = flags;
}

struct gpio_pin *gpio_chip_to_pin(struct gpio_chip *chip, uint16_t nr)
{
	if(!chip)
		return NULL;

	return &chip->pins[nr];
}

int gpio_pin_request(struct gpio_pin *pin)
{
	struct gpio_chip *chip;
	unsigned long flags;
	int err;

	chip = pin->chip;
	spin_lock_irqsave(&chip->lock, flags);
	if(test_and_set_bit(GPIO_REQUESTED, &pin->flags))
		err = -EINVAL;
	else
		err = -EOK;
	spin_unlock_irqrestore(&chip->lock, flags);

	return err;
}

#define pin_is_requested(__p) test_bit(GPIO_REQUESTED, &(__p)->flags)

int gpio_direction_input(struct gpio_pin *pin)
{
	struct gpio_chip *chip;
	unsigned long flags;
	int err;

	chip = pin->chip;
	spin_lock_irqsave(&chip->lock, flags);
	if(!pin_is_requested(pin)) {
		spin_unlock_irqrestore(&chip->lock, flags);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&chip->lock, flags);

	err = chip->direction_input(chip, pin->nr);
	if(!err)
		clear_bit(GPIO_IS_OUTPUT, &pin->flags);

	return err;
}

int gpio_direction_output(struct gpio_pin *pin, int value)
{
	if(test_bit(GPIO_ACTIVE_LOW, &pin->flags))
		value = !value;

	return raw_gpio_direction_ouput(pin, value);
}

int raw_gpio_direction_ouput(struct gpio_pin *pin, int value)
{
	struct gpio_chip *chip;
	unsigned long flags;
	int err;

	if(value && test_bit(GPIO_OPEN_DRAIN, &pin->flags))
		return gpio_direction_input(pin);

	if(!value && test_bit(GPIO_OPEN_SOURCE, &pin->flags))
		return gpio_direction_input(pin);

	chip = pin->chip;
	spin_lock_irqsave(&chip->lock, flags);
	if(!pin_is_requested(pin)) {
		spin_unlock_irqrestore(&chip->lock, flags);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&chip->lock, flags);

	err = chip->direction_output(chip, value, pin->nr);
	if(!err)
		set_bit(GPIO_IS_OUTPUT, &pin->flags);

	return err;
}

int raw_gpio_pin_write(struct gpio_pin *pin, int val)
{
	struct gpio_chip *chip;
	unsigned long flags;

	chip = pin->chip;
	spin_lock_irqsave(&chip->lock, flags);
	if(!pin_is_requested(pin)) {
		spin_unlock_irqrestore(&chip->lock, flags);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&chip->lock, flags);

	if(test_bit(GPIO_IS_OUTPUT, &pin->flags))
		return chip->set(chip, val, pin->nr);
	else
		return -EINVAL;
}

int gpio_pin_write(struct gpio_pin *pin, int val)
{
	if(test_bit(GPIO_ACTIVE_LOW, &pin->flags))
		val = !val;

	if(test_bit(GPIO_ACTIVE_LOW, &pin->flags) && val)
		return gpio_direction_input(pin);

	if(test_bit(GPIO_OPEN_DRAIN, &pin->flags) && !val)
		return gpio_direction_input(pin);

	return raw_gpio_pin_write(pin, val);
}


int gpio_pin_read(struct gpio_pin *pin)
{
	int val;

	if(!pin)
		return -EOK;

	val = raw_gpio_read_pin(pin);
	if(test_bit(GPIO_ACTIVE_LOW, &pin->flags))
		val = !val;

	return val;
}

int raw_gpio_read_pin(struct gpio_pin *pin)
{
	struct gpio_chip *chp;

	chp = pin->chip;
	return chp->get ? chp->get(chp, pin->nr) : false;
}

