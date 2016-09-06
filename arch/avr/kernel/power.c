/*
 *  ETA/OS - AVR power management
 *  Copyright (C) 2016   Michel Megens <dev@michelmegens.net>
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
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/power.h>

#include <asm/io.h>

static int avr_pwr_mode = 0;

static uint8_t avr_power_mode_to_mask(int mode)
{
	uint8_t mask = 0;

	switch(mode) {
	case POWER_IDLE:
		mask = 0;
		break;
	case POWER_STANDBY:
		mask = BIT(SM2) | BIT(SM1);
		break;
	case POWER_SAVE:
		mask = BIT(SM0) | BIT(SM1);
		break;
	case POWER_DOWN:
		mask = BIT(SM1);
		break;
	default:
		break;
	}

	return mask;
}

void arch_set_power_mode(int mode)
{
	uint8_t mask;

	switch(mode) {
	case POWER_IDLE:
	case POWER_STANDBY:
	case POWER_SAVE:
	case POWER_DOWN:
		avr_pwr_mode = mode;
		break;
	default:
		mode = 0;
		return;
	}

	mask = avr_power_mode_to_mask(mode);
	mask |= SMCR & BIT(SE);
	SMCR = mask;
}

void arch_hibernate(void)
{
	SMCR |= BIT(SE);

	__asm__ __volatile__(
			"sleep"
			:
			:
			:
			);
	SMCR &= ~BIT(SE);
	barrier();
}

