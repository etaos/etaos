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

/**
 * @addtogroup usart-atmega
 */
/* @{ */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/usart.h>
#include <etaos/error.h>
#include <etaos/spinlock.h>
#include <etaos/bitops.h>
#include <etaos/init.h>

#include <asm/io.h>
#include <asm/usart.h>

static int atmega_usart_putc(struct usart *usart, int c)
{
	if(c == '\n')
		atmega_usart_putc(usart, '\r');

	while(( UCSR0A & BIT(UDRE0) ) == 0);
	UCSR0A |= BIT(TXCn);
	UDR0 = c;

	return c;
}

static int atmega_usart_getc(struct usart *usart)
{
	int c;

	while(!(UCSR0A & BIT(RXC0)));
	c = UDR0;
	return c;
}

static int atmega_usart_read(struct usart *uart, void *rx, size_t rxlen)
{
	unsigned char *rxbuff;
	size_t i;

	if(!rx)
		return -EINVAL;

	rxbuff = rx;
	for(i = 0; i < rxlen; i++) {
		rxbuff[i] = atmega_usart_getc(uart);
	}

	return -EOK;
}

static int atmega_usart_write(struct usart *uart, const void *tx,
			size_t txlen)
{
	int err;
	size_t i;
	const char *txbuf = tx;

	err = 0;
	for(i = 0; i < txlen; i++) {
		err = atmega_usart_putc(uart, txbuf[i]);
		if(err != txbuf[i]) {
			err = -EINVAL;
			break;
		}
	}
	
	if(err < 0)
		return 0;
	else
		return err;
}

static struct usart atmega_usart = {
	.putc = atmega_usart_putc,
	.getc = atmega_usart_getc,
	.write = atmega_usart_write,
	.read = atmega_usart_read,
	.timeout = 0,
	.dev = { .name = "atm-usart", },
};

/**
 * @brief Initialise the ATmega USART.
 */
static __used void atmega_usart_init(void)
{
	UBRR0H = UBRR0H_VALUE;
	UBRR0L = UBRR0L_VALUE;
	UCSR0A &= ~(BIT(U2X0));
	UCSR0C = BIT(UCSZ01) | BIT(UCSZ00);
	UCSR0B = BIT(TXEN0) | BIT(RXEN0);

	usart_initialise(&atmega_usart);
#ifdef CONFIG_STDIO_USART
	setup_usart_streams(&atmega_usart);
#endif
}

module_init(atmega_usart_init);

/* @} */

