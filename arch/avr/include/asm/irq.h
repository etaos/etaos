/*
 *  ETA/OS - AVR IRQ definitions
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

#ifndef __AVR_IRQ_H__
#define __AVR_IRQ_H__

#define IRQ_ENTRY_ATTRIBS signal, used, externally_visible

#define irq_vector(num) __vector_ ## num

#define TIMER0_OVERFLOW_VECTOR_NUM 16
#define SPI_STC_VECTOR_NUM	   17
#define TWI_STC_VECTOR_NUM	   24

#define TIMER0_OVERFLOW_VECTOR irq_vector(16)
#define SPI_STC_VECTOR irq_vector(17)
#define TWI_STC_VECTOR irq_signal(24)

#define SIGNAL(_vector) \
	void _vector(void) __attribute__((IRQ_ENTRY_ATTRIBS)); \
	void _vector(void)

#endif /* __AVR_IRQ_H__ */

