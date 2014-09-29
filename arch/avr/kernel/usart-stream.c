/*
 *  ETA/OS - STDIO streams of the USART
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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
#include <etaos/stdio.h>

#include <asm/io.h>
#include <asm/usart.h>

static int avr_usart_putc(int c, FILE stream)
{
	if(c == '\n')
		avr_usart_putc('\r', stream);

	while(( UCSR0A & BIT(UDRE0) ) == 0);
	UCSR0A |= BIT(TXCn);
	UDR0 = c;

	return c;
}

static FDEV_SETUP_STREAM(usart_stream,
			 NULL,
			 NULL,
			 &avr_usart_putc,
			 NULL,
			 NULL,
			 "SIMAVR_STREAM",
			 _FDEV_SETUP_RW,
			 NULL);

void avr_setup_usart_streams(void)
{
	UBRR0H = UBRR0H_VALUE;
	UBRR0L = UBRR0L_VALUE;
	UCSR0A &= ~(BIT(U2X0));
	UCSR0C = BIT(UCSZ01) | BIT(UCSZ00);
	UCSR0B = BIT(TXEN0);

	stdout = &usart_stream;
	stdin = &usart_stream;
	stderr = &usart_stream;
}
