/*
 *  ETA/OS - GPIO driver
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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
 * @addtogroup gpiolib
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/spinlock.h>
#include <etaos/gpio.h>
#include <etaos/bitops.h>
#include <etaos/init.h>

/**
 * @brief GPIO system chip.
 *
 * The GPIO system chip is a pointer to the GPIO chip descriptor of the CPU.
 */
struct gpio_chip *gpio_sys_chip;

/**
 * @brief Initialise a new GPIO chip.
 * @param chip Chip which has to be initialised
 * @param pins Amount of pins that belong to \p chip.
 * @return An error code, -EOK on success.
 */
int gpio_chip_init(struct gpio_chip *chip, uint16_t pins)
{
	int err;

	chip->pins = kmalloc(sizeof(*chip->pins) * pins);
	chip->num = pins;
	if(!chip->pins)
		return -ENOMEM;
	else
		err = -EOK;

	spinlock_init(&chip->lock);

	return err;
}

/**
 * @brief Initialise a new pin descriptor.
 * @param chip The chip where the pin belongs to.
 * @param pin The pin that has to be initialised.
 * @param nr GPIO pin number.
 * @param flags GPIO pin flags.
 */
void gpio_pin_init(struct gpio_chip *chip, 
		struct gpio_pin *pin, uint16_t nr, unsigned long flags)
{
	if(!pin)
		return;

	clear_bit(GPIO_REQUESTED, &flags);
	pin->chip = chip;
	pin->flags = flags;
	pin->nr = nr;
}

/**
 * @brief Get a pin descriptor based on its pin number.
 * @param chip GPIO chip where the pin belongs to.
 * @param nr Pin number.
 * @return The GPIO pin descriptor.
 */
struct gpio_pin *gpio_chip_to_pin(struct gpio_chip *chip, uint16_t nr)
{
	if(!chip)
		return NULL;

	return &chip->pins[nr];
}

/**
 * @brief Request a pin for usage.
 * @param pin Pin to request.
 * @return Error code, zero on success.
 * @warning ALL pins have to be requested before usage.
 */
int gpio_pin_request(struct gpio_pin *pin)
{
	struct gpio_chip *chip;
	unsigned long flags;
	int err;

	chip = pin->chip;
	spin_lock_irqsave(&chip->lock, flags);
	if(test_and_set_bit(GPIO_REQUESTED, &pin->flags)) {
		err = -EINVAL;
	} else {
		err = -EOK;
	}
	spin_unlock_irqrestore(&chip->lock, flags);

	return err;
}

int gpio_pin_release(struct gpio_pin *pin)
{
	struct gpio_chip *chip;
	unsigned long flags;
	int err;

	chip = pin ? pin->chip : NULL;
	if(!chip)
		return -EINVAL;

	spin_lock_irqsave(&chip->lock, flags);
	if(test_and_clear_bit(GPIO_REQUESTED, &pin->flags)) {
		err = -EOK;
	} else {
		err = 1;
	}
	spin_unlock_irqrestore(&chip->lock, flags);

	return err;
}

#define pin_is_requested(__p) test_bit(GPIO_REQUESTED, &(__p)->flags)

/**
 * @brief Configure a pin as input.
 * @param pin Pin to configure as input.
 * @return An error code.
 */
int gpio_direction_input(struct gpio_pin *pin)
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

	return __raw_gpio_direction_input(pin);
}

/**
 * @brief Configure a pin as input.
 * @param pin Pin to configure as input.
 * @note Does not do any pin allocation checks.
 * @return An error code.
 */
int __raw_gpio_direction_input(struct gpio_pin *pin)
{
	int err;
	struct gpio_chip *chip = pin->chip;

	err = chip->direction_input(pin->chip, pin->nr);
	if(!err)
		clear_bit(GPIO_IS_OUTPUT, &pin->flags);

	return err;
}

/**
 * @brief Set a pin to OUTPUT.
 * @param pin Pin to configure as output.
 * @param value Value to write to the pin.
 * @return Error code. Zero on success.
 */
int gpio_direction_output(struct gpio_pin *pin, int value)
{
	if(test_bit(GPIO_ACTIVE_LOW, &pin->flags))
		value = !value;

	return raw_gpio_direction_ouput(pin, value);
}

/**
 * @brief Set a pin to OUTPUT.
 * @param pin Pin to configure as output.
 * @param value Value to write to the pin.
 * @return Error code. Zero on success.
 */
int raw_gpio_direction_ouput(struct gpio_pin *pin, int value)
{
	struct gpio_chip *chip;
	unsigned long flags;

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

	return __raw_gpio_direction_output(pin, value);

}

/**
 * @brief Set a pin to OUTPUT.
 * @param pin Pin to configure as output.
 * @param value Value to write to the pin.
 * @note Does not do any pin allocation checks.
 * @return Error code. Zero on success.
 */
int __raw_gpio_direction_output(struct gpio_pin *pin, int value)
{
	int err;
	struct gpio_chip *chip = pin->chip;

	err = chip->direction_output(pin->chip, value, pin->nr);
	if(!err)
		set_bit(GPIO_IS_OUTPUT, &pin->flags);

	return err;
}

/**
 * @brief Write a boolean value to a GPIO pin.
 * @param pin Pin to write to.
 * @param val Value to write.
 * @return Error code. Zero on success.
 * @note This function doesn't take GPIO_ACTIVE_LOW into account.
 */
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

	return __raw_gpio_pin_write(pin, val);
}

/**
 * @brief Write a boolean value to a GPIO pin.
 * @param pin Pin to write to.
 * @param val Value to write.
 * @return Error code. Zero on success.
 * @note Does not do any pin allocation checks.
 * @note This function doesn't take GPIO_ACTIVE_LOW into account.
 */
int __raw_gpio_pin_write(struct gpio_pin *pin, int val)
{
	struct gpio_chip *chip = pin->chip;

	if(test_bit(GPIO_IS_OUTPUT, &pin->flags))
		return chip->set(chip, val, pin->nr);
	else
		return -EINVAL;
}

/**
 * @brief Write a boolean value to a GPIO pin.
 * @param pin Pin to write to.
 * @param val Value to write.
 * @return Error code. Zero on success.
 */
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

/**
 * @brief Read the value of a gpio pin.
 * @param pin Pin to read.
 * @return Value of \p pin.
 */
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

/**
 * @brief Get the direction of a GPIO pin.
 * @param pin Pin to get the direction of.
 * @return The direction.
 * @retval GPIO_DIR_IN if the direction is input.
 * @retval GPIO_DIR_OUT if the direction is output.
 */
int gpio_get_direction(struct gpio_pin *pin)
{
	int status = -EINVAL;
	struct gpio_chip *chip;

	chip = pin->chip;
	if(!chip || chip->get_direction)
		return status;

	status = chip->get_direction(chip, pin->nr);
	if(status > 0) {
		status = GPIO_DIR_IN;
		clear_bit(GPIO_IS_OUTPUT, &pin->flags);
	} else if(!status) {
		status = GPIO_DIR_OUT;
		set_bit(GPIO_IS_OUTPUT, &pin->flags);
	}

	return status;
}

/**
 * @brief Read the value of a gpio pin.
 * @param pin Pin to read.
 * @return Value of \p pin.
 */
int raw_gpio_read_pin(struct gpio_pin *pin)
{
	struct gpio_chip *chp;

	chp = pin->chip;
	return chp->get ? chp->get(chp, pin->nr) : false;
}

static void __used gpiolib_init(void)
{
}

subsys_init(gpiolib_init);

/* @} */

