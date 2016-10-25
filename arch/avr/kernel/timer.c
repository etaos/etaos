/*
 *  ETA/OS - AVR timers
 *  Copyright (C) 2014, 2015, 2016   Michel Megens <dev@bietje.net>
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
#include <etaos/bitops.h>
#include <etaos/tick.h>
#include <etaos/error.h>
#include <etaos/time.h>
#include <etaos/delay.h>
#include <etaos/math.h>
#include <etaos/init.h>

#include <asm/timer.h>
#include <asm/irq.h>
#include <asm/io.h>

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
 * @return An error code.
 * @note Dummy function, the AVR sysclk is enabled on initialise.
 */
static int avr_sysclk_enable(struct clocksource *cs)
{
	return -EOK;
}

/**
 * @brief Start the AVR system clock.
 * @param irq IRQ vector number.
 * @param src Clocksource structure for the AVR sysclk.
 */
static void avr_start_sysclk(int irq, struct clocksource *src)
{
	systick_setup(irq, src);
#if F_CPU == 16000000
	OCR0A = 250;
#elif F_CPU == 8000000
	OCRA0 = 125;
#else
#error Unsupported CPU frequency for timer IRQ
#endif
	TIMSK0 = TOIE0;
	TCCR0A = WGM00 | WGM01;
	TCCR0B = WGM02 | CS00 | CS01;
}

/**
 * @brief Start the AVR timers.
 * @see avr_sysclk_enable avr_start_sysclk
 */
static void __used avr_timer_init(void)
{
	clocksource_init(sysclk.name, &sysclk, AVR_SYSCLK_FRQ,
					&avr_sysclk_enable, NULL);
	avr_start_sysclk(TIMER0_OVERFLOW_VECTOR_NUM, &sysclk);
	sysctl(SYS_SET_SYSCLK, &sysclk);
}

#ifdef CONFIG_DELAY_US
static inline double us_to_loops(double us)
{
	return (1.19205297 * us) + 0.11920527;
}

void arch_delay_us(double __us)
{
	unsigned long loops = 0UL;

	/*
	 * Calculate the number of delay loops using the following
	 * formula:
	 *
	 * f(x) = 1324503.311t + 0.1324503
	 * Where:
	 *  - f(x) is the number of delay loops required
	 *  - t is the delay time in seconds
	 */
	loops = us_to_loops(__us);
	delay_loop(loops);
}
#endif

#ifdef CONFIG_IRQ_DEBUG
unsigned long test_sys_tick = 0UL;
#endif

SIGNAL(TIMER0_OVERFLOW_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();

#ifdef CONFIG_IRQ_DEBUG
	test_sys_tick++;

	if(test_sys_tick == 5000UL) {
		chip->chip_handle(EXT_IRQ0_NUM);
		test_sys_tick = 0UL;
	}
#endif

	chip->chip_handle(TIMER0_OVERFLOW_VECTOR_NUM);
}

subsys_init(avr_timer_init);

/* @} */

