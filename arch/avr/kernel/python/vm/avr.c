/*
 *  ETA/OS - Python AVR library
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

#include <etaos/python.h>
#include <asm/io.h>

static volatile unsigned char *pm_port_to_port(int32_t port)
{
	switch(port) {
	case 1:
		return &PORTA;
	case 2:
		return &PORTB;
	case 3:
		return &PORTC;
	case 4:
		return &PORTD;
	case 5:
		return &PORTE;
	case 6:
		return &PORTF;
	case 7:
		return &PORTG;
	case 8:
		return &PORTH;
#ifdef PORTI
	case 9:
		return &PORTI;
#endif
#ifdef PORTJ
	case 10:
		return &PORTJ;
#endif
#ifdef PORTK
	case 11:
		return &PORTK;
#endif
#ifdef PORTL
	case 12:
		return &PORTL;
#endif
	default:
		return NULL;
	}
}

static volatile unsigned char *pm_port_to_pin(int32_t port)
{
	switch(port) {
	case 1:
		return &PINA;
	case 2:
		return &PINB;
	case 3:
		return &PINC;
	case 4:
		return &PIND;
	case 5:
		return &PINE;
	case 6:
		return &PINF;
	case 7:
		return &PING;
	case 8:
		return &PINH;
#ifdef PORTI
	case 9:
		return &PINI;
#endif
#ifdef PORTJ
	case 10:
		return &PINJ;
#endif
#ifdef PORTK
	case 11:
		return &PINK;
#endif
#ifdef PORTL
	case 12:
		return &PINL;
#endif
	default:
		return NULL;
	}
}


static volatile unsigned char *pm_port_to_ddr(int32_t port)
{
	switch(port) {
	case 1:
		return &DDRB;
	case 2:
		return &DDRB;
	case 3:
		return &DDRC;
	case 4:
		return &DDRD;
	case 5:
		return &DDRE;
	case 6:
		return &DDRF;
	case 7:
		return &DDRG;
	case 8:
		return &DDRH;
#ifdef PORTI
	case 9:
		return &DDRI;
#endif
#ifdef PORTJ
	case 10:
		return &DDRJ;
#endif
#ifdef PORTK
	case 11:
		return &DDRK;
#endif
#ifdef PORTL
	case 12:
		return &DDRL;
#endif
	default:
		return NULL;
	}
}

volatile unsigned char *pm_port_to_addr(int32_t port, port_type_t type)
{
	switch(type) {
	case PORT:
		return pm_port_to_port(port);
	case PIN:
		return pm_port_to_pin(port);
	case DDR:
		return pm_port_to_ddr(port);
	default:
		return NULL;
	}
}

