/*
 *  ETA/OS - AVR time
 *  Copyright (C) 2014   Michel Megens
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
 * @addtogroup atmega
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/time.h>

#include <asm/time.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/cpu.h>

/**
 * @def AVR_SYSCLK_FRQ
 * @brief Frequency of the AVR system clock.
 */
#define AVR_SYSCLK_FRQ 1000UL

static struct clocksource sysclk = {
	.name = "sys-clk",
};

/**
 * @brief Getter for the ATmega AVR system clock.
 * @return The ATmega AVR system clock.
 * @see avr_start_sysclk
 */
struct clocksource *avr_get_sys_clk(void)
{
	return &sysclk;
}

/**
 * @brief Enable the AVR system clock.
 * @param cs Clocksource which has to be enabled.
 * @note Dummy function, the AVR sysclk is enabled on initialise.
 */
static int avr_sysclk_enable(struct clocksource *cs)
{
	return -EOK;
}

/**
 * @brief Start the AVR timers.
 * @see avr_sysclk_enable avr_start_sysclk
 */
void avr_timer_init(void)
{
	tm_clock_source_initialise(sysclk.name, &sysclk, AVR_SYSCLK_FRQ,
					&avr_sysclk_enable, NULL);
	avr_start_sysclk(TIMER0_OVERFLOW_VECTOR_NUM, &sysclk);
	sysctl(SYS_SET_SYSCLK, &sysclk);
}

/* @} */

