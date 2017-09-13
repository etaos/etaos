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

/**
 * @addtogroup pwm
 * @{
 */

#ifndef __PWM_H__
#define __PWM_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/clocksource.h>
#include <etaos/gpio.h>

/**
 * @brief Channel duty cycle.
 */
typedef union {
	float fraction; //!< Duty cycle as a fraction of the period.
	uint16_t time; //!< Duty cycle measured in Hertz.
} duty_cycle_t;

#define PWM_ENABLED_FLAG             0 //!< Enable flag.
#define PWM_DUTY_CYCLE_FRACTION_FLAG 1 //!< Set if duty cyle is given as a fraction.
#define PWM_PIN_HIGH_FLAG            2 //!< Set if the PWM channel is currently high.

/**
 * @brief PWM channel settings.
 */
struct pwm_state {
	unsigned long flags; //!< PWM flags.
	duty_cycle_t cycle; //!< Duty cycle.
};

struct pwm;
/**
 * @brief PWM channel data.
 */
struct pwm_channel {
	struct pwm *base; //!< PWM chip to which this channel belongs.

	struct gpio_pin *output; //!< Output pin.
	struct pwm_state state; //!< Current channel settings.
};

/**
 * @brief PWM chip driver.
 */
struct pwm_driver {
	/**
	 * @brief Start the PWM.
	 * @param pwm PWM to start.
	 * @param freq PWM frequency.
	 */
	void (*start)(struct pwm *pwm,uint32_t freq);

	/**
	 * @brief Stop the PWM.
	 * @param pwm PWM to stop.
	 */
	void (*stop)(struct pwm*);

	/**
	 * @brief Update channel features.
	 * @param pwm PWM chip to which \p chanid belongs.
	 * @param chanid Channel ID of the channel to edit.
	 * @param state New channel settings.
	 */
	void (*update_channel)(struct pwm *pwm, int chanid, struct pwm_state *state);

	/**
	 * @brief Update the channel frequency.
	 * @param pwm PWM chip.
	 * @param freq Frequency to tune.
	 * @return An error code.
	 */
	int (*update_frequency)(struct pwm *pwm, uint32_t freq);
};

/**
 * @brief PWM chip datastructure.
 *
 * This structure describes a single device capable of generating
 * PWM.
 */
struct pwm {
	spinlock_t lock; //!< Lock.
	struct clocksource *src; //!< Backend clock.
	void *iobase; //!< Memory I/O..
	uint8_t num; //!< Number of channels..
	uint32_t frequency; //!< Signal frequency in Hertz.
	struct pwm_driver *driver; //!< PWM driver.

	struct pwm_channel *channels[]; //!< PWM channels.
};

CDECL
extern struct pwm *pwm_get_chip(int id);
extern void pwm_register_chip(struct pwm *pwm, int id);
extern void pwm_update_channel(struct pwm *pwm, int chanid, struct pwm_state *state);
extern void pwm_start(struct pwm *pwm, uint32_t frequency);
extern void pwm_stop(struct pwm *pwm);
extern int pwm_update_frequency(struct pwm *pwm, uint32_t frequency);

#endif

/** @} */
