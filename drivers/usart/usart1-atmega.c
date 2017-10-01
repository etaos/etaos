/*
 *  ETA/OS - ATmega USART1
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
#include <etaos/stdio.h>
#include <etaos/mutex.h>
#include <etaos/usart.h>
#include <etaos/error.h>
#include <etaos/spinlock.h>
#include <etaos/bitops.h>
#include <etaos/init.h>

#include <asm/io.h>
#include <asm/usart.h>

static mutex_t usart_mtx;

static const uint8_t *usart_tx_buff;
static size_t usart_tx_len;
static size_t usart_tx_idx;

static uint8_t *usart_rx_buff;
static size_t usart_rx_len;
static size_t usart_rx_idx;

static int atmega_usart_putc(struct usart *usart, int c)
{
	irq_enter_critical();
	while(( UCSR1A & BIT(UDRE1) ) == 0);
	UCSR1A |= BIT(TXCn);
	UDR1 = c;
	irq_exit_critical();

	return c;
}

static int atmega_usart_getc(struct usart *usart)
{
	int c = 0;

	irq_enter_critical();
	usart_rx_buff = (uint8_t*)&c;
	usart_rx_len = 1;
	usart_rx_idx = 0;
	irq_exit_critical();

	mutex_wait(&usart_mtx);
	return c;
}

static int atmega_usart_read(struct usart *uart, void *rx, size_t rxlen)
{
	if(!rx)
		return -EINVAL;

	irq_enter_critical();
	usart_rx_buff = rx;
	usart_rx_len = rxlen;
	usart_rx_idx = 0;
	irq_exit_critical();

	mutex_wait(&usart_mtx);
	return -EOK;
}

static int atmega_usart_write(struct usart *uart, const void *tx,
			size_t txlen)
{
	if(!tx)
		return -EINVAL;

	irq_enter_critical();
	usart_tx_buff = tx;
	usart_tx_len = txlen;
	usart_tx_idx = 0;
	UCSR1B |= BIT(UDRIE1);
	irq_exit_critical();
	mutex_wait(&usart_mtx);

	return txlen;
}

static irqreturn_t usart_rx_irq(struct irq_data *data, void *arg)
{
	if(!usart_rx_len || !usart_rx_buff || usart_rx_idx >= usart_rx_len)
		return IRQ_HANDLED;

	if(usart_rx_idx < usart_rx_len) {
		usart_rx_buff[usart_rx_idx] = UDR1;
		usart_rx_idx++; /* AVRICC breaks if we don't do this
				   seperately */
	}

	if(usart_rx_idx >= usart_rx_len) {
		usart_rx_idx = 0;
		mutex_unlock_from_irq(&usart_mtx);
	}

	return IRQ_HANDLED;
}

static irqreturn_t usart_udre_irq(struct irq_data *data, void *arg)
{
	if(!usart_tx_buff || !usart_tx_len || usart_tx_idx >= usart_tx_len)
		return IRQ_HANDLED;

	UDR1 = usart_tx_buff[usart_tx_idx];
	usart_tx_idx++;

	if(usart_tx_idx >= usart_tx_len) {
		usart_tx_idx = 0;
		mutex_unlock_from_irq(&usart_mtx);
		UCSR1B &= ~BIT(UDRIE1);
	}

	return IRQ_HANDLED;
}

static struct usart atmega_usart1 = {
	.putc = atmega_usart_putc,
	.getc = atmega_usart_getc,
	.write = atmega_usart_write,
	.read = atmega_usart_read,
	.timeout = 0,
	.dev = { .name = "atm-usart1", },
};

static __used void atmega_usart1_init(void)
{
	uint16_t baud;

	usart_rx_buff = NULL;
	usart_rx_len = 0;
	usart_rx_idx = 0;
	usart_tx_buff = NULL;
	usart_tx_len = 0;
	usart_tx_idx = 0;

	baud = (F_CPU / 4 / CONFIG_USART1_BAUD - 1) / 2;
	UCSR1A |= BIT(U2X1);

	if(((F_CPU == 16000000UL) && (CONFIG_USART1_BAUD == 57600)) || baud > 4095) {
		UCSR1A = 0;
		baud = (F_CPU / 8 / CONFIG_USART1_BAUD - 1) / 2;
	}

	UBRR1 = baud;
	UCSR1C = BIT(UCSZ11) | BIT(UCSZ10);
	UCSR1B = BIT(RXEN1) | BIT(TXEN1) | BIT(RXCIE1);

	mutex_init(&usart_mtx);

	usart_initialise(&atmega_usart1);
	irq_request(USART1_RX_COMPLETE_VECTOR_NUM, &usart_rx_irq, IRQ_FALLING_MASK,
			&atmega_usart1);
	irq_request(USART1_UDRE_VECTOR_NUM, &usart_udre_irq, IRQ_FALLING_MASK,
			&atmega_usart1);
}

module_init(atmega_usart1_init);
