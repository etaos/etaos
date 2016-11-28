/*
 *  ETA/OS - AVR IRQ entry points
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

#include <asm/irq.h>
#include <asm/asmdef.h>
#include <asm/io.h>

.extern i2c_stc_irq
.extern spi_stc_irq
.extern asm_preempt_schedule_irq

#if defined(CONFIG_I2C_ATMEGA) || defined(CONFIG_I2C_ATMEGA_MODULE)
.global TWI_STC_VECTOR
.type TWI_STC_VECTOR, @function
TWI_STC_VECTOR:
	__irq_save__
	XCALL i2c_stc_irq
	__irq_restore__ asm_preempt_schedule_irq
	ret
#endif

#if defined(CONFIG_ATMEGA_SPI) || defined(CONFIG_ATMEGA_SPI_MODULE)
.global SPI_STC_VECTOR
.type SPI_STC_VECTOR, @function
SPI_STC_VECTOR:
	__irq_save__
	XCALL spi_stc_irq
	__irq_restore__ asm_preempt_schedule_irq
	ret
#endif

