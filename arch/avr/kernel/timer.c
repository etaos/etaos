/*
 *  ETA/OS - AVR timers
 *  Copyright (C) 2014, 2015, 2016   Michel Megens <dev@bietje.net>
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
#include <etaos/preempt.h>

#include <asm/timer.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/watchdog.h>

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
	clocksource_init(sysclk.name, &sysclk, AVR_SYSCLK_FRQ);
	avr_start_sysclk(TIMER0_OVERFLOW_VECTOR_NUM, &sysclk);
	sysctl(SYS_SET_SYSCLK, &sysclk);
}

#ifdef CONFIG_DELAY_US
static inline double us_to_loops(double us)
{
	return (1.19205297 * us) + 0.11920527;
}

/* Delay us as taken from the Arduino project. All copyright goes to them. */
void arch_delay_us(unsigned int us)
{
	/* call = 4 cycles + 2 to 4 cycles to init us(2 for constant delay, 4 for variable)

	calling avrlib's delay_us() function with low values (e.g. 1 or
	2 microseconds) gives delays longer than desired. */
#if F_CPU >= 24000000L
	/* for the 24 MHz clock for the aventurous ones, trying to overclock */

	if (!us)
		return;

	/* the following loop takes a 1/6 of a microsecond (4 cycles)
	   per iteration, so execute it six times for each microsecond of
	   delay requested. */
	us *= 6; // x6 us, = 7 cycles

	/* account for the time taken in the preceeding commands.
	   we just burned 22 (24) cycles above, remove 5, (5*4=20)
	   us is at least 6 so we can substract 5 */
	us -= 5; //=2 cycles

#elif F_CPU >= 20000000L
	/* for the 20 MHz clock on rare Arduino boards */

	/* for a one-microsecond delay, simply return.  the overhead
	   of the function call takes 18 (20) cycles, which is 1us */
	__asm__ __volatile__ (
		"nop" "\n\t"
		"nop" "\n\t"
		"nop" "\n\t"
		"nop");
	if (us <= 1)
		return;

	/* the following loop takes a 1/5 of a microsecond (4 cycles)
	   per iteration, so execute it five times for each microsecond of
	   delay requested.*/
	us = (us << 2) + us; // x5 us, = 7 cycles

	/* account for the time taken in the preceeding commands.
	   we just burned 26 (28) cycles above, remove 7, (7*4=28)
	   us is at least 10 so we can substract 7 */
	us -= 7; // 2 cycles

#elif F_CPU >= 16000000L
	/* for the 16 MHz clock on most Arduino boards

	   for a one-microsecond delay, simply return.  the overhead
	   of the function call takes 14 (16) cycles, which is 1us */
	if (us <= 1)
		return;

	/* the following loop takes 1/4 of a microsecond (4 cycles)
	   per iteration, so execute it four times for each microsecond of
	   delay requested. */
	us <<= 2; // x4 us, = 4 cycles

	/* account for the time taken in the preceeding commands.
	   we just burned 19 (21) cycles above, remove 5, (5*4=20)
	   us is at least 8 so we can substract 5 */
	us -= 5;

#elif F_CPU >= 12000000L
	/* for the 12 MHz clock if somebody is working with USB

	   for a 1 microsecond delay, simply return.  the overhead
	   of the function call takes 14 (16) cycles, which is 1.5us */
	if (us <= 1)
		return;

	/* the following loop takes 1/3 of a microsecond (4 cycles)
	   per iteration, so execute it three times for each microsecond of
	   delay requested. */
	us = (us << 1) + us;

	/* account for the time taken in the preceeding commands.
	   we just burned 20 (22) cycles above, remove 5, (5*4=20)
	   us is at least 6 so we can substract 5 */
	us -= 5;

#elif F_CPU >= 8000000L
	/* for the 8 MHz internal clock

	   for a 1 and 2 microsecond delay, simply return.  the overhead
	   of the function call takes 14 (16) cycles, which is 2us */
	if (us <= 2)
		return;

	/* the following loop takes 1/2 of a microsecond (4 cycles)
	   per iteration, so execute it twice for each microsecond of
	   delay requested. */
	us <<= 1;

	/* account for the time taken in the preceeding commands.
	   we just burned 17 (19) cycles above, remove 4, (4*4=16)
	   us is at least 6 so we can substract 4 */
	us -= 4;

#else
	/* for the 1 MHz internal clock (default settings for common
	   Atmega microcontrollers) */

	/* the overhead of the function calls is 14 (16) cycles */
	if (us <= 16)
		return;

	if (us <= 25)
		return;

	us -= 22;
	/* the following loop takes 4 microseconds (4 cycles)
	   per iteration, so execute it us/4 times
	   us is at least 4, divided by 4 gives us 1 (no zero delay bug) */
	us >>= 2;
#endif

	/* busy wait */
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}
#endif

#ifdef CONFIG_IRQ_DEBUG
unsigned short test_sys_tick = 0;
#endif

SIGNAL(TIMER0_OVERFLOW_VECTOR)
{
	struct watchdog *wdog;
	struct irq_chip *chip = arch_get_irq_chip();

	wdog = watchdog_get_info();
	if(wdog && wdog->enabled)
		watchdog_reset();

#ifdef CONFIG_IRQ_DEBUG
	test_sys_tick++;

	if(test_sys_tick == 5000) {
		chip->chip_handle(EXT_IRQ0_VECTOR_NUM);
		test_sys_tick = 0;
	}
#endif
	chip->chip_handle(TIMER0_OVERFLOW_VECTOR_NUM);
	return;
}

subsys_init(avr_timer_init);

/* @} */
