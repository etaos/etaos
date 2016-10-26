/*
 *  ETA/OS - AVR ATmega delay loop
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

__tmp_reg__  = 0
__zero_reg__ = 1
.section .text
.global delay_loop
/* void delay_loop(unsigned long num); */
.type delay_loop, @function
delay_loop:
.TOP_LOOP:
	cp r22, __zero_reg__
	cpc r23, __zero_reg__
	cpc r24, __zero_reg__
	cpc r25, __zero_reg__
	breq .LOOP_END

	/* NOP instruction */
	mov __zero_reg__, __zero_reg__

	subi r22, 1
	sbc r23, __zero_reg__
	sbc r24, __zero_reg__
	sbc r25, __zero_reg__
	rjmp .TOP_LOOP

.LOOP_END:
	ret

