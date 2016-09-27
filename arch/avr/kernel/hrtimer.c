/*
 *  ETA/OS - AVR time
 *  Copyright (C) 2014, 2015   Michel Megens
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
#include <etaos/types.h>
#include <etaos/bitops.h>
#include <etaos/error.h>
#include <etaos/hrtimer.h>
#include <etaos/delay.h>
#include <etaos/init.h>

#include <asm/timer.h>
#include <asm/irq.h>
#include <asm/io.h>

#define AVR_HRTIMER_FREQ 2000

static struct hrtimer_source avr_hrtimer_src = {
	.base = {
		.name = "avr-hr-clock",
	},
};

static void avr_start_hrclock(int irq, struct clocksource *src)
{
	hrtimer_init(irq, src);
#if F_CPU == 16000000
	OCR2A = 250;
#elif F_CPU == 8000000
	OCR20 = 125;
#else
#error Unsupported CPU frequency for HR clock
#endif
	TIMSK2 = BIT(TOIE2);
	TCCR2A = BIT(WGM20) | BIT(WGM21);
	TCCR2B = BIT(WGM22) | BIT(CS20) | BIT(CS21);
}

static void __used avr_hrtimer_init(void)
{
	hrtimer_source_init(avr_hrtimer_src.base.name, &avr_hrtimer_src,
			NULL, NULL, AVR_HRTIMER_FREQ);
	sysctl(SYS_SET_HR_CLK, &avr_hrtimer_src.base);
	avr_start_hrclock(TIMER2_OVERFLOW_VECTOR_NUM, &avr_hrtimer_src.base);
}

SIGNAL(TIMER2_OVERFLOW_VECTOR)
{
	struct irq_chip *chip = arch_get_irq_chip();
	chip->chip_handle(TIMER2_OVERFLOW_VECTOR_NUM);
}

subsys_init(avr_hrtimer_init);

