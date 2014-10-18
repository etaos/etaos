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

#ifndef __GPIO__H__
#define __GPIO__H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/device.h>

struct gpio_pin {
	struct gpio_chip *chip;
	uint16_t nr;
	unsigned long flags;
};

struct gpio_chip {
	struct gpio_pin *pins;
	uint16_t num;

	struct device dev;
	spinlock_t lock;
	
	int (*direction_output)(struct gpio_chip *chip, int val, uint16_t nr);
	int (*direction_input)(struct gpio_chip *chip, uint16_t nr);
	int (*get)(struct gpio_chip *chip, uint16_t nr);
	int (*set)(struct gpio_chip *chip, int val, uint16_t nr);
};

#define GPIO_REQUESTED		0 //!< If set, the pin is requested.
#define GPIO_IS_OUTPUT		1 //!< Defines whether the pin is output or not
#define GPIO_TRIG_FALL		2 //!< IRQ triggers on falling edge.
#define GPIO_TRIG_RISE		3 //!< IRQ triggers on rising edge.
#define GPIO_ACTIVE_LOW		4 //!< Pin is active low (AKA !value).
#define GPIO_OPEN_DRAIN		5 //!< Pin is connected to an open drain.
#define GPIO_OPEN_SOURCE	6 //!< Pin is connected to an open source.
#define GPIO_IRQ		7 //!< If set, the pin is connected to an IRQ.

extern void gpio_pin_init(struct gpio_pin *pin, unsigned long flags);
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

#endif

