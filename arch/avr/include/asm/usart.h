/*
 *  ETA/OS - ATmega USART bus controller
 *  Copyright (C) 2014   Michel Megens
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

#ifndef __ATMEGA_USART_H
#define __ATMEGA_USART_H

#include <etaos/kernel.h>
#include <etaos/usart.h>

#include <asm/io.h>

#define BAUD CONFIG_USART_BAUD

#define U2X0 1
#define UCSZ00 1
#define UCSZ01 2
#define TXEN0 3
#define RXEN0 4
#define UDRE0 5
#define TXCn 6
#define RXC0 7

// interrupts
#define RXCIE0 7
#define TXCIE0 6
#define UDRIE0 5

CDECL
extern void avr_setup_usart_streams(void);
CDECL_END

#endif /* __ATMEGA_USART_H */
