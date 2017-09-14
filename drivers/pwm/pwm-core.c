/*
 *  ETA/OS - PWM core
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/time.h>
#include <etaos/clocksource.h>
#include <etaos/pwm.h>
#include <etaos/mem.h>
#include <etaos/irq.h>
#include <etaos/init.h>
#include <etaos/stdio.h>

#define MAX_PWM_CHIPS 6

static struct pwm *pwm_chips[MAX_PWM_CHIPS];

/**
 * @brief Register a new PWM chip.
 * @param pwm PWM to register.
 * @param id PWM id.
 */
void pwm_register_chip(struct pwm *pwm, int id)
{
	pwm_chips[id] = pwm;
}

/**
 * @brief Search for a registered PWM device.
 * @param id ID to search for.
 * @return The PWM for \p id.
 * @retval NULL If \p id is unknown.
 */
struct pwm *pwm_get_chip(int id)
{
	if(id < MAX_PWM_CHIPS)
		return pwm_chips[id];
	else
		return NULL;
}

/**
 * @brief Update a PWM channel.
 * @param pwm PWM device.
 * @param chanid Channel ID to update.
 * @param state PWM state to apply to \p chanid.
 */
void pwm_update_channel(struct pwm *pwm, int chanid, struct pwm_state *state)
{
	unsigned long flags;

	spin_lock_irqsave(&pwm->lock, flags);
	pwm->driver->update_channel(pwm, chanid, state);
	spin_unlock_irqrestore(&pwm->lock, flags);
}

/**
 * @brief Start a PWM device.
 * @param pwm PWM to start.
 * @param frequency Frequency to start \p pwm on.
 */
void pwm_start(struct pwm *pwm, uint32_t frequency)
{
	unsigned long flags;

	spin_lock_irqsave(&pwm->lock, flags);
	pwm->driver->start(pwm, frequency);
	spin_unlock_irqrestore(&pwm->lock, flags);
}

/**
 * @brief Stop a PWM device.
 * @param pwm PWM to stop.
 * @note All PWM channels of \p pwm will be stopped.
 */
void pwm_stop(struct pwm *pwm)
{
	unsigned long flags;

	spin_lock_irqsave(&pwm->lock, flags);
	pwm->driver->stop(pwm);
	spin_unlock_irqrestore(&pwm->lock, flags);
}

/**
 * @brief Update the frequency of a PWM device.
 * @param pwm PWM to update.
 * @param frequency Frequency to tune \p pwm to.
 * @return An error code.
 */
int pwm_update_frequency(struct pwm *pwm, uint32_t frequency)
{
	unsigned long flags;
	int rc;

	spin_lock_irqsave(&pwm->lock, flags);
	rc = pwm->driver->update_frequency(pwm, frequency);
	spin_unlock_irqrestore(&pwm->lock, flags);

	return rc;
}

/** @} */
