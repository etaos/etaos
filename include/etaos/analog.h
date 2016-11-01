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
 * @file include/etaos/analog.h ADC definitions
 */

#ifndef __ANALOG_H__
#define __ANALOG_H__

/**
 * @addtogroup analog
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/device.h>

/**
 * @brief Analog pin structure.
 */
struct analog_pin {
	struct analog_chip *chip; //!< Chip to which the pin belongs.
	uint8_t num; //!< Pin number.
};

struct analog_chip {
	struct analog_pin *pins; //!< Analog pin descriptor array.
	int num; //!< Length of the \p pins array.
	struct analog_pin *selected_pin; //!< Currently select pin.
	struct device dev; //!< ADC device.

	/**
	 * @brief Read from the ADC device.
	 * @param pin Pin to read from.
	 * @return The value read from the ADC device.
	 */
	int (*get)(struct analog_pin *pin);
	/**
	 * @brief Configure the ADC device.
	 * @param chip Chip to configure.
	 * @param reg Configuration register.
	 * @param arg Argument to \p reg.
	 * @return An error code.
	 */
	int (*ioctl)(struct analog_chip *chip, unsigned long reg, void *arg);
};

/**
 * @brief Analog ioctl registers.
 */
typedef enum {
	ANALOG_SELECT_PIN, //!< Select a new analog pin.
} analog_ioctl_t;

/**
 * @name Analog pins
 * @{
 */
#define PIN_A0		0 //!< Pin 0
#define PIN_A1		1 //!< Pin 1
#define PIN_A2		2 //!< Pin 2
#define PIN_A3		3 //!< Pin 3
#define PIN_A4		4 //!< Pin 4
#define PIN_A5		5 //!< Pin 5
#define PIN_A6		6 //!< Pin 6
#define PIN_A7		7 //!< Pin 7
#define PIN_A8		8 //!< Pin 8
#define PIN_A9		9 //!< Pin 9
#define PIN_A10		10 //!< Pin 10
#define PIN_A11		11 //!< Pin 11
#define PIN_A12		12 //!< Pin 12
#define PIN_A13		13 //!< Pin 13
#define PIN_A14		14 //!< Pin 14
#define PIN_A15		15 //!< Pin 15
/** @} */

CDECL
/**
 * @brief Get the name of an ADC.
 * @param chip ADC chip to get the name of.
 * @return The name as registered in the device tree.
 */
static inline const char *analog_chip_to_name(struct analog_chip *chip)
{
	if(!chip)
		return NULL;

	return chip->dev.name;
}

static inline struct analog_pin *analog_chip_to_pin(struct analog_chip *chip,
		int pin)
{
	if(chip->num <= pin)
		return NULL;

	return &chip->pins[pin];
}

extern int analog_read(struct analog_pin *pin);
extern int analog_chip_init(struct analog_chip *chip);
CDECL_END

extern struct analog_chip *analog_syschip;

/** @} */

#endif

