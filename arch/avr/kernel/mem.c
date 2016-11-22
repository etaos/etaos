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

static void avr_sre_enable(char reg, char idx)
{
	switch(reg) {
	case 0:
		DDRA |= BIT(idx);
		PORTA &= ~BIT(idx);
		break;
	case 1:
		DDRB |= BIT(idx);
		PORTB &= ~BIT(idx);
		break;
	case 2:
		DDRC |= BIT(idx);
		PORTC &= ~BIT(idx);
		break;
	case 3:
		DDRD |= BIT(idx);
		PORTD &= ~BIT(idx);
		break;
	case 4:
		DDRE |= BIT(idx);
		PORTE &= ~BIT(idx);
		break;
	case 5:
		DDRF |= BIT(idx);
		PORTF &= ~BIT(idx);
		break;
	case 6:
		DDRG |= BIT(idx);
		PORTG &= ~BIT(idx);
		break;
	case 7:
		DDRH |= BIT(idx);
		PORTH &= ~BIT(idx);
		break;
	case 8:
		DDRJ |= BIT(idx);
		PORTJ &= ~BIT(idx);
		break;
	case 9:
		DDRK |= BIT(idx);
		PORTK &= ~BIT(idx);
		break;
	case 10:
		DDRL |= BIT(idx);
		PORTL &= ~BIT(idx);
		break;
	default:
		break;
	}
}

/**
 * @brief Enable the external SRAM interface.
 *
 * SRE stands for <b>SR</b>AM <b>e</b>nable.
 */
void avr_sre(void)
{
	char idx,
	     pin = CONFIG_EXT_MEM_ENABLE_PIN;

	/* Enable the external memory interface */
	XMCRA = BIT(SRE);

	idx = pin % 8;
	pin = pin / 8;

	avr_sre_enable(pin, idx);

	/* Set bank selection pins to output */
	DDRL |= BIT(P5) | BIT(P6) | BIT(P7);
	/* Select bank 0 */
	PORTL &= ~(BIT(P5) | BIT(P6) | BIT(P7));
}

/** @} */

