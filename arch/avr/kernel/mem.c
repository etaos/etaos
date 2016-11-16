/*
 *  ETA/OS - ATmega memory
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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
#include <etaos/bitops.h>
#include <etaos/mem.h>

#include <asm/io.h>

/**
 * @brief Enable the external SRAM interface.
 *
 * SRE stands for <b>SR</b>AM <b>e</b>nable.
 */
void avr_sre(void)
{
	/* Enable the external memory interface */
	XMCRA = BIT(SRE);

	/* Enable the RAM device */
	DDRD |= BIT(P7);
	PORTD &= ~BIT(P7);

	/* Set bank selection pins to output */
	DDRL |= BIT(P5) | BIT(P6) | BIT(P7);
	/* Select bank 0 */
	PORTL &= ~(BIT(P5) | BIT(P6) | BIT(P7));
}

/** @} */

