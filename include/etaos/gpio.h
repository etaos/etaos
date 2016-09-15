/*
 *  ETA/OS - GPIO driver
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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
 * @file etaos/gpio.h
 */

/**
 * @addtogroup gpiolib
 */
/* @{ */

#ifndef __GPIO__H__
#define __GPIO__H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/device.h>
#include <etaos/bitops.h>

/**
 * @struct gpio_pin
 * @brief GPIO pin descriptor.
 */
struct gpio_pin {
	struct gpio_chip *chip; //!< GPIO chip where the pin belongs to.
	uint16_t nr; //!< Pin numerator.
	unsigned long flags; //!< Pin flags.
};

/**
 * @struct gpio_chip
 * @brief GPIO chip descriptor.
 *
 * A GPIO chip is the descriptor of the chip controlling a number of GPIO
 * pins.
 */
struct gpio_chip {
	struct gpio_pin *pins; //!< Pin descriptor array.
	uint16_t num; //!< Lengths of \p pins.

	spinlock_t lock; //!< Concurrency lock.

	/**
	 * @brief Set a pin to output.
	 * @param chip GPIO chip.
	 * @param val Value to write.
	 * @param nr Pin configure.
	 */
	int (*direction_output)(struct gpio_chip *chip, int val, uint16_t nr);

	/**
	 * @brief Set a pin to input.
	 * @param chip GPIO chip.
	 * @param nr Pin configure.
	 */
	int (*direction_input)(struct gpio_chip *chip, uint16_t nr);

	/**
	 * @brief Get the direction of a pin.
	 * @param chip GPIO chip.
	 * @param nr Pin to get the direction of.
	 */
	int (*get_direction)(struct gpio_chip *chip, uint16_t nr);

	/**
	 * @brief Get the value of a pin.
	 * @param chip GPIO chip.
	 * @param nr Pin to get the value from.
	 */
	int (*get)(struct gpio_chip *chip, uint16_t nr);

	/**
	 * @brief Set the value of a pin.
	 * @param chip GPIO chip.
	 * @param nr Pin to set.
	 */
	int (*set)(struct gpio_chip *chip, int val, uint16_t nr);
};

/**
 * @name GPIO pin flags
 */
/* @{ */
#define GPIO_REQUESTED		0 //!< If set, the pin is requested.
#define GPIO_IS_OUTPUT		1 //!< Defines whether the pin is output or not
#define GPIO_TRIG_FALL		2 //!< IRQ triggers on falling edge.
#define GPIO_TRIG_RISE		3 //!< IRQ triggers on rising edge.
#define GPIO_ACTIVE_LOW		4 //!< Pin is active low (AKA !value).
#define GPIO_OPEN_DRAIN		5 //!< Pin is connected to an open drain.
#define GPIO_OPEN_SOURCE	6 //!< Pin is connected to an open source.
#define GPIO_IRQ		7 //!< If set, the pin is connected to an IRQ.
/* @} */

CDECL
extern void gpio_pin_init(struct gpio_chip *chp,
		struct gpio_pin *pin, uint16_t nr, 
		unsigned long flags);
extern int gpio_chip_init(struct gpio_chip *chip, uint16_t pins);
extern struct gpio_pin *gpio_chip_to_pin(struct gpio_chip *chip, uint16_t nr);
extern int gpio_pin_request(struct gpio_pin *pin);
extern int gpio_pin_write(struct gpio_pin *pin, int val);
extern int gpio_pin_read(struct gpio_pin *pin);
extern int gpio_direction_input(struct gpio_pin *pin);
extern int gpio_direction_output(struct gpio_pin *pin, int value);
extern int raw_gpio_direction_ouput(struct gpio_pin *pin, int value);
extern int raw_gpio_pin_write(struct gpio_pin *pin, int val);
extern int raw_gpio_read_pin(struct gpio_pin *pin);
extern int gpio_get_direction(struct gpio_pin *pin);
extern int gpio_pin_release(struct gpio_pin *pin);

/**
 * @brief Configure a pin as open drain.
 * @param pin Pin to configure.
 */
static inline void gpio_set_open_drain(struct gpio_pin *pin)
{
	if(test_bit(GPIO_OPEN_SOURCE, &pin->flags))
		clear_bit(GPIO_OPEN_SOURCE, &pin->flags);

	set_bit(GPIO_OPEN_DRAIN, &pin->flags);
}

/**
 * @brief Configure a pin as open source.
 * @param pin Pin to configure.
 */
static inline void gpio_set_open_source(struct gpio_pin *pin)
{
	if(test_bit(GPIO_OPEN_DRAIN, &pin->flags))
		clear_bit(GPIO_OPEN_DRAIN, &pin->flags);

	set_bit(GPIO_OPEN_SOURCE, &pin->flags);
}

extern struct gpio_chip *gpio_sys_chip;

/**
 * @brief Set the system chip.
 * @param chip Chip to set as system chip.
 * @see gpio_sys_chip
 */
static inline void gpio_set_sys_chip(struct gpio_chip *chip)
{
	gpio_sys_chip = chip;
}

CDECL_END
/**
 * @name GPIO pin direction flags
 */
/* @{ */
#define GPIO_DIR_OUT false //!< Direction output flag.
#define GPIO_DIR_IN  true  //!< Direction input flag.
/* @} */

#endif
/* @} */

