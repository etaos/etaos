/*
 *  ETA/OS - SIMUL AVR support
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
#include <asm/simulavr.h>

int simul_avr_write_byte(int c, FILE stream)
{
	if(c == '\n')
		simul_avr_write_byte('\r', stream);

	SIMO = c;
	return c;
}

void simul_avr_write_string(char *s, FILE stream)
{
	while(*s) {
		simul_avr_write_byte(*s, stream);
		s++;
	}
}
