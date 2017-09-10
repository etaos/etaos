/*
 *  ETA/OS - PWM device
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

#ifndef __PWM_H__
#define __PWM_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/clocksource.h>
#include <etaos/gpio.h>

typedef union {
	float fraction;
	uint16_t time;
} duty_cycle_t;

#define PWM_ENABLED_FLAG             0
#define PWM_DUTY_CYCLE_FRACTION_FLAG 1
#define PWM_PIN_HIGH_FLAG            2

struct pwm_state {
	unsigned long flags;
	duty_cycle_t cycle;
};

struct pwm;
struct pwm_channel {
	struct pwm *base;

	struct gpio_pin *output;
	struct pwm_state state;
};

struct pwm_driver {
	void (*start)(struct pwm*,uint32_t);
	void (*stop)(struct pwm*);
	void (*update_channel)(struct pwm *, int, struct pwm_state*);
	int (*update_frequency)(struct pwm*, uint32_t);
};

struct pwm {
	spinlock_t lock;
	struct clocksource *src;
	void *iobase;
	uint8_t num;
	uint32_t frequency;
	struct pwm_driver *driver;

	struct pwm_channel *channels[];
};

CDECL
extern struct pwm *pwm_get_chip(int id);
extern void pwm_register_chip(struct pwm *pwm, int id);
extern void pwm_update_channel(struct pwm *pwm, int chanid, struct pwm_state *state);
extern void pwm_start(struct pwm *pwm, uint32_t frequency);
extern void pwm_stop(struct pwm *pwm);
extern int pwm_update_frequency(struct pwm *pwm, uint32_t frequency);

#endif
