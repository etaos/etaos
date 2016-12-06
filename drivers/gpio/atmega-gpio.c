/*
 *  ETA/OS - ATmega GPIO driver
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
 * @addtogroup atmega-gpio
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/gpio.h>
#include <etaos/init.h>
#include <etaos/mem.h>

#include <asm/io.h>
#include <asm/pgm.h>

/**
 * @def PINS_PER_PORT
 * @brief Amount of pins per I/O port
 */
#define PINS_PER_PORT 8

static const size_t __pgm atmega_gpio_ports[] = {
#ifdef PORTA
	(size_t)&PORTA,
#endif
#ifdef PORTB
	(size_t)&PORTB,
#endif
#ifdef PORTC
	(size_t)&PORTC,
#endif
#ifdef PORTD
	(size_t)&PORTD,
#endif
#ifdef PORTE
	(size_t)&PORTE,
#endif
#ifdef PORTF
	(size_t)&PORTF,
#endif
#ifdef PORTG
	(size_t)&PORTG,
#endif
#ifdef PORTH
	(size_t)&PORTH,
#endif
#ifdef PORTJ
	(size_t)&PORTJ,
#endif
#ifdef PORTK
	(size_t)&PORTK,
#endif
#ifdef PORTL
	(size_t)&PORTL,
#endif
};

static const size_t __pgm atmega_gpio_pins[] = {
#ifdef PORTA
	(size_t)&PINA,
#endif
#ifdef PORTB
	(size_t)&PINB,
#endif
#ifdef PORTC
	(size_t)&PINC,
#endif
#ifdef PORTD
	(size_t)&PIND,
#endif
#ifdef PORTE
	(size_t)&PINE,
#endif
#ifdef PORTF
	(size_t)&PINF,
#endif
#ifdef PORTG
	(size_t)&PING,
#endif
#ifdef PORTH
	(size_t)&PINH,
#endif
#ifdef PORTJ
	(size_t)&PINJ,
#endif
#ifdef PORTK
	(size_t)&PINK,
#endif
#ifdef PORTL
	(size_t)&PINL,
#endif
};

static const size_t __pgm atmega_gpio_ddrs[] = {
#ifdef PORTA
	(size_t)&DDRA,
#endif
#ifdef PORTB
	(size_t)&DDRB,
#endif
#ifdef PORTC
	(size_t)&DDRC,
#endif
#ifdef PORTD
	(size_t)&DDRD,
#endif
#ifdef PORTE
	(size_t)&DDRE,
#endif
#ifdef PORTF
	(size_t)&DDRF,
#endif
#ifdef PORTG
	(size_t)&DDRG,
#endif
#ifdef PORTH
	(size_t)&DDRH,
#endif
#ifdef PORTJ
	(size_t)&DDRJ,
#endif
#ifdef PORTK
	(size_t)&DDRK,
#endif
#ifdef PORTL
	(size_t)&DDRL,
#endif
};

static inline volatile uint8_t *atmega_pin_to_ddr(struct gpio_pin *pin)
{
	uint8_t idx;

	idx = pin->nr / PINS_PER_PORT;
	return (volatile uint8_t*)pgm_read_word(atmega_gpio_ddrs+idx);
}

static inline volatile uint8_t *atmega_pin_to_pin_addr(struct gpio_pin *pin)
{
	uint8_t idx;

	idx = pin->nr / PINS_PER_PORT;
	return (volatile uint8_t*)pgm_read_word(atmega_gpio_pins+idx);
}

static inline volatile uint8_t *atmega_pin_to_port(struct gpio_pin *pin)
{
	uint8_t idx;

	idx = pin->nr / PINS_PER_PORT;
	return (volatile uint8_t*)pgm_read_word(atmega_gpio_ports+idx);
}

static inline uint8_t atmega_pin_index(struct gpio_pin *pin)
{
	return (uint8_t)(pin->nr % PINS_PER_PORT);
}

static int atmega_set_pin(struct gpio_chip *chip, int val, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;

	irq_enter_critical();
	pin = chip->pins[nr];
	idx = atmega_pin_to_port(pin);
	bit = atmega_pin_index(pin);

	if(val)
		*idx |= 1 << bit;
	else
		*idx &= ~(1 << bit);
	irq_exit_critical();

	return 0;
}

static int atmega_get_pin(struct gpio_chip *chip, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;
	int retval;

	irq_enter_critical();
	pin = chip->pins[nr];
	idx = atmega_pin_to_pin_addr(pin);
	bit = atmega_pin_index(pin);

	retval = *idx;
	retval &= (1<<bit);
	irq_exit_critical();

	return !!retval;
}

static int atmega_dir_out(struct gpio_chip *chip, int val, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;

	irq_enter_critical();
	pin = chip->pins[nr];
	idx = atmega_pin_to_ddr(pin);
	bit = atmega_pin_index(pin);

	*idx |= 1 << bit;
	irq_exit_critical();

	return atmega_set_pin(chip, val, nr);
}

static int atmega_dir_in(struct gpio_chip *chip, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;

	irq_enter_critical();
	pin = chip->pins[nr];
	idx = atmega_pin_to_ddr(pin);
	bit = atmega_pin_index(pin);

	*idx &= ~(1 << bit);
	irq_exit_critical();

	return 0;
}

static int atmega_get_dir(struct gpio_chip *chip, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;
	int dir;

	irq_enter_critical();
	pin = chip->pins[nr];
	idx = atmega_pin_to_ddr(pin);
	bit = atmega_pin_index(pin);

	dir = *idx;
	dir &= 1 << bit;
	irq_exit_critical();

	return !dir;
}

static struct gpio_chip atmega_gpio_chip = {
	.get = &atmega_get_pin,
	.set = &atmega_set_pin,
	.direction_output = &atmega_dir_out,
	.direction_input = &atmega_dir_in,
	.get_direction = &atmega_get_dir,
};

/**
 * @brief Initialise the ATmega AVR GPIO chip.
 * @note Also sets the gpio_sys_chip.
 * @see gpio_sys_chip
 */
void atmega_init_gpio(void)
{
	int err;
#ifndef CONFIG_GPIO_LOW_FOOTPRINT
	struct gpio_pin *pin;
#endif

	err = gpio_chip_init(&atmega_gpio_chip, GPIO_PINS);

	if(!err) {
#ifndef CONFIG_GPIO_LOW_FOOTPRINT
		for(err = 0; err < GPIO_PINS; err++) {
			pin = kzalloc(sizeof(*pin));
			gpio_pin_init(&atmega_gpio_chip, pin, err, 0);
			atmega_gpio_chip.pins[err] = pin;
		}
#endif
	} else {
		return;
	}

	gpio_set_sys_chip(&atmega_gpio_chip);
	return;
}

module_init(atmega_init_gpio);

/* @} */

