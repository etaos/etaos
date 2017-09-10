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

void pwm_register_chip(struct pwm *pwm, int id)
{
	pwm_chips[id] = pwm;
}

struct pwm *pwm_get_chip(int id)
{
	return pwm_chips[id];
}

void pwm_update_channel(struct pwm *pwm, int chanid, struct pwm_state *state)
{
	unsigned long flags;

	spin_lock_irqsave(&pwm->lock, flags);
	pwm->driver->update_channel(pwm, chanid, state);
	spin_unlock_irqrestore(&pwm->lock, flags);
}

void pwm_start(struct pwm *pwm, uint32_t frequency)
{
	unsigned long flags;

	spin_lock_irqsave(&pwm->lock, flags);
	pwm->driver->start(pwm, frequency);
	spin_unlock_irqrestore(&pwm->lock, flags);
}

void pwm_stop(struct pwm *pwm)
{
	unsigned long flags;

	spin_lock_irqsave(&pwm->lock, flags);
	pwm->driver->stop(pwm);
	spin_unlock_irqrestore(&pwm->lock, flags);
}

int pwm_update_frequency(struct pwm *pwm, uint32_t frequency)
{
	unsigned long flags;
	int rc;

	spin_lock_irqsave(&pwm->lock, flags);
	rc = pwm->driver->update_frequency(pwm, frequency);
	spin_unlock_irqrestore(&pwm->lock, flags);

	return rc;
}
