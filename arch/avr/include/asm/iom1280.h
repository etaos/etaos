/*
 *  ETA/OS ATmega1280 IO defs
 *  Copyright (C) 2012   Michel Megens
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

#ifndef __IO1280_H__
#ifndef __IO1280_H__

#define RAMEND 0x1FFF
#define INTERNAL_RAMEND RAMEND
#define RAMSTART 0x200

/* interrupt defs */
#define CONFIG_ARCH_VECTORS 57

#define _VECTORS_SIZE (57*4)

#define TIMER0_OVERFLOW_VECTOR_NUM 16
#define SPI_STC_VECTOR_NUM	   17
#define TWI_STC_VECTOR_NUM	   24

#define TIMER0_OVERFLOW_VECTOR irq_vector(23)
#define SPI_STC_VECTOR irq_vector(24)
#define TWI_STC_VECTOR irq_vector(39)

#define AVR_IRQ_FLAG 7
#define AVR_IRQ_BITS (1 << AVR_IRQ_FLAG)

/* TIMER 0 */ 
#define TCCR0A MEM_IO8(0x44)
#define WGM00  0x1
#define WGM01  0x2
#define COM0B0 0x10
#define COM0B1 0x20
#define COM0A0 0x40
#define COM0A1 0x80

#define TCCR0B MEM_IO8(0x45)
#define CS00  0x1
#define CS01  0x2
#define CS02  0x4
#define WGM02 0x8
#define FOC0B 0x40
#define FOC0A 0x80

#define TCNT0  MEM_IO8(0x46)
#define OCR0A  MEM_IO8(0x47)
#define OCR0B  MEM_IO8(0x48)

#define TIMSK0 MEM_IO8(0x6E)
#define TOIE0  0x1
#define OCIE0A 0x2
#define OCIE0B 0x4

#define TIFR0  MEM_IO8(0x35)
#define TOV   0x1
#define OCF0A 0x2
#define OCF0B 0x4

/* GPIO defs */
#define PINA MEM_IO8(0x20)
#define DDRA MEM_IO8(0x21)
#define PORTA MEM_IO8(0x22)

#define PINB MEM_IO8(0x23)
#define DDRB MEM_IO8(0x24)
#define PORTB MEM_IO8(0x25)

#define PINC MEM_IO8(0x26)
#define DDRC MEM_IO8(0x27)
#define PORTC MEM_IO8(0x28)

#define PIND MEM_IO8(0x29)
#define DDRD MEM_IO8(0x2A)
#define PORTD MEM_IO8(0x2B)

#define PINE MEM_IO8(0x2C)
#define DDRE MEM_IO8(0x2D)
#define PORTE MEM_IO8(0x2E)

#define PINF MEM_IO8(0x2F)
#define DDRF MEM_IO8(0x30)
#define PORTF MEM_IO8(0x31)

#define PING MEM_IO8(0x32)
#define DDRG MEM_IO8(0x33)
#define PORTG MEM_IO8(0x34)

/* USART defs */
#define UBRR0L MEM_IO8(0xC4)
#define UBRR0H MEM_IO8(0xC5)
#define UDR0 MEM_IO8(0xC6)

#define UCSR0A MEM_IO8(0xC0)
#define UCSR0B MEM_IO8(0xC1)
#define UCSR0C MEM_IO8(0xC2)

/* TWI defs */
#define TWBR  MEM_IO8(0xB8)
#define TWCR  MEM_IO8(0xBC)
#define TWINT	7
#define TWEA	6
#define TWSTA	5
#define TWSTO	4
#define TWWC	3
#define TWEN	2
#define TWIE	0

#define TWSR  MEM_IO8(0xB9)
#define TWPS1	1
#define TWPS0	0
#define I2C_NOINFO 0xF8

#define TWDR  MEM_IO8(0xBB)
#define TWAR  MEM_IO8(0xBA)
#define TWGCE 	0

#define TWAMR MEM_IO8(0xBD)

#if F_CPU == 8000000
#define TWBR_VAL 32
#define TWSR_VAL (TWSR & 0xF8)
#elif F_CPU == 16000000
#define TWBR_VAL 72
#define TWSR_VAL (TWSR & 0xF8)
#endif

#endif /* __IO1280_H__ */

