/*
 *  ETA/OS - EEPROM functions
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
#include <etaos/error.h>
#include <etaos/sched.h>
#include <etaos/stdlib.h>

#include <asm/io.h>
#include <asm/eeprom.h>

static inline int eeprom_is_ready(void)
{
	return !(EECR & BIT(EEPE));
}

static void eeprom_wait_busy(void)
{
	if(unlikely(eeprom_is_ready()))
		return;

	while(!eeprom_is_ready());
}

static void eeprom_wait_nonbusy(void)
{
	if(unlikely(eeprom_is_ready()))
		return;

	while(!eeprom_is_ready()) {
#ifdef CONFIG_SCHED
		schedule();
#endif
	}
}

void eeprom_write_block(uint16_t addr, const void *buff, size_t num)
{
	size_t idx;
	const uint8_t *buffer = buff;

	eeprom_wait_nonbusy();

	for(idx = 0; idx < num; idx++) {
		eeprom_wait_busy();
		EEAR = addr + idx;
		EEDR = buffer[idx];
		EECR |= BIT(EEMPE);
		EECR |= BIT(EEPE);
	}
}

void eeprom_read_block(uint16_t addr, void *buff, size_t num)
{
	size_t idx;
	uint8_t *buffer = buff;

	eeprom_wait_nonbusy();

	for(idx = 0; idx < num; idx++) {
		eeprom_wait_busy();
		EEAR = addr + idx;
		EECR |= BIT(EERE);
		buffer[idx] = EEDR;
	}
}
