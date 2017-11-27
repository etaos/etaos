/*
 *  ETA/OS - Watchdog driver
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
#include <etaos/irq.h>
#include <etaos/stdlib.h>
#include <etaos/stdio.h>

#include <asm/pgm.h>
#include <asm/io.h>
#include <asm/watchdog.h>

struct watchdog wdt = {
	.enabled = false,
	.ms_delay = 0,
};

static __noinit uint8_t mcusr_copy;

static __init3 __naked __used void watchdog_boot_reset(void)
{
	mcusr_copy = MCUSR;
	MCUSR = 0;
	watchdog_disable();
}

void watchdog_enable(const uint8_t value)
{
	__asm__ __volatile__ (
	"in __tmp_reg__, __SREG__"	"\n\t"
	"cli"				"\n\t"
	"wdr"				"\n\t"
	"sts %0, %1"			"\n\t"
	"out __SREG__, __tmp_reg__"	"\n\t"
	"sts %0, %2"			"\n\t"
	:
	: "M" (WDTCSR_ADDR),
	  "r" ((uint8_t) (BIT(WDCE) | BIT(WDE))),
	  "r" ((uint8_t) ((value & 0x8 ? WDP3 : 0x0) | BIT(WDE) |
	       BIT(WDIE) | (value & 0x7)))
	: "r0"
	);

	wdt.enabled = true;
	wdt.ms_delay = -1;
}

void watchdog_disable(void)
{
	register uint8_t tmp;

	__asm__ __volatile__(
	"in __tmp_reg__, __SREG__"	"\n\t"
	"cli"				"\n\t"
	"wdr"				"\n\t"
	"lds %0, %1"			"\n\t"
	"ori %0, %2"			"\n\t"
	"sts %1, %0"			"\n\t"
	"sts %1, __zero_reg__"		"\n\t"
	"out __SREG__, __tmp_reg__"	"\n\t"
	: "=d" (tmp)
	: "M" (WDTCSR_ADDR),
	  "M" ((uint8_t)(BIT(WDCE) | BIT(WDE)))
	: "r0"
	);

	wdt.enabled = false;
}

SIGNAL(WDT_TMO_VECTOR)
{
	fprintf_P(stderr, PSTR("[WDT]: Watchdog timeout triggered!\n"));
}
