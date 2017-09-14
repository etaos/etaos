/*
 *  ETA/OS - AVR PWM timers
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
#include <etaos/bitops.h>
#include <etaos/error.h>
#include <etaos/time.h>
#include <etaos/clocksource.h>
#include <etaos/init.h>
#include <etaos/irq.h>

#include <asm/timer.h>
#include <asm/irq.h>
#include <asm/io.h>

#ifdef CONFIG_PWM0
static struct clocksource pwm0_clk = {
	.name = "pwm0"
};

struct clocksource *pwm_get_clock0(void)
{
	return &pwm0_clk;
}

static void avr_pwm_init_clk0(void)
{
	TCCR1B = BIT(WGM13);
	ICR1 = 8000; /* Setup for 1KHz */
	TCNT1 = 0;
}

SIGNAL(TIMER1_OCA_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCA_VECTOR_NUM);
}

SIGNAL(TIMER1_OCB_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCB_VECTOR_NUM);
}

#ifdef COMC
SIGNAL(TIMER1_OCC_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCC_VECTOR_NUM);
}
#endif

#else
#define avr_pwm_init_clk0()
#endif

#ifdef CONFIG_PWM1
static struct clocksource pwm1_clk = {
	.name = "pwm1"
};

struct clocksource *pwm_get_clock1(void)
{
	return &pwm1_clk;
}

static void avr_pwm_init_clk1(void)
{
	TCCR3B = BIT(WGM33);
	ICR3 = 8000; /* Setup for 1KHz */
	TCNT3 = 0;
}

SIGNAL(TIMER3_OCA_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCA_VECTOR_NUM);
}

SIGNAL(TIMER3_OCB_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCB_VECTOR_NUM);
}

#ifdef COMC
SIGNAL(TIMER3_OCC_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCC_VECTOR_NUM);
}
#endif
#else
#define avr_pwm_init_clk1()
#endif

#ifdef CONFIG_PWM2
static struct clocksource pwm2_clk = {
	.name = "pwm2"
};

struct clocksource *pwm_get_clock2(void)
{
	return &pwm2_clk;
}

static void avr_pwm_init_clk2(void)
{
	TCCR4B = BIT(WGM43);
	ICR4 = 8000; /* Setup for 1KHz */
	TCNT4 = 0;
}

SIGNAL(TIMER4_OCA_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCA_VECTOR_NUM);
}

SIGNAL(TIMER4_OCB_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCB_VECTOR_NUM);
}

#ifdef COMC
SIGNAL(TIMER4_OCC_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCC_VECTOR_NUM);
}
#endif
#else
#define avr_pwm_init_clk2()
#endif

#ifdef CONFIG_PWM3
static struct clocksource pwm3_clk = {
	.name = "pwm3"
};

struct clocksource *pwm_get_clock3(void)
{
	return &pwm3_clk;
}

static void avr_pwm_init_clk3(void)
{
	TCCR5B = BIT(WGM53);
	ICR5 = 8000; /* Setup for 1KHz */
	TCNT5 = 0;
}

SIGNAL(TIMER5_OCA_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCA_VECTOR_NUM);
}

SIGNAL(TIMER5_OCB_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCB_VECTOR_NUM);
}

#ifdef COMC
SIGNAL(TIMER5_OCC_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER1_OCC_VECTOR_NUM);
}
#endif
#else
#define avr_pwm_init_clk3()
#endif

static void __used avr_pwm_init_clocks(void)
{
	avr_pwm_init_clk0();
	avr_pwm_init_clk1();
	avr_pwm_init_clk2();
	avr_pwm_init_clk3();
}

subsys_init(avr_pwm_init_clocks);
