/*
 *  Eta/OS - ETA/OS STL CPU
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

#ifndef __STL_CPU_H__
#define __STL_CPU_H__

#include <etaos/types.h>
#include <etaos/gpio.h>
#include <etaos/analog.h>

#include <asm/io.h>

class CPU {
public:
#if GPIO_PINS >= 24
	static const unsigned short pin1 = 0;
	static const unsigned short pin2 = 1;
	static const unsigned short pin3 = 2;
	static const unsigned short pin4 = 3;
	static const unsigned short pin5 = 4;
	static const unsigned short pin6 = 5;
	static const unsigned short pin7 = 6;
	static const unsigned short pin8 = 7;
	static const unsigned short pin9 = 8;
	static const unsigned short pin10 = 9;
	static const unsigned short pin11 = 10;
	static const unsigned short pin12 = 11;
	static const unsigned short pin13 = 12;
	static const unsigned short pin14 = 13;
	static const unsigned short pin15 = 14;
	static const unsigned short pin16 = 15;
	static const unsigned short pin17 = 16;
	static const unsigned short pin18 = 17;
	static const unsigned short pin19 = 18;
	static const unsigned short pin20 = 19;
	static const unsigned short pin21 = 20;
	static const unsigned short pin22 = 21;
	static const unsigned short pin23 = 22;
	static const unsigned short pin24 = 23;
#endif
#if GPIO_PINS >= 54
	static const unsigned short pin25 = 24;
	static const unsigned short pin26 = 25;
	static const unsigned short pin27 = 26;
	static const unsigned short pin28 = 27;
	static const unsigned short pin29 = 28;
	static const unsigned short pin30 = 29;
	static const unsigned short pin31 = 30;
	static const unsigned short pin32 = 31;
	static const unsigned short pin33 = 32;
	static const unsigned short pin34 = 33;
	static const unsigned short pin35 = 34;
	static const unsigned short pin36 = 35;
	static const unsigned short pin37 = 36;
	static const unsigned short pin38 = 37;
	static const unsigned short pin39 = 38;
	static const unsigned short pin40 = 39;
	static const unsigned short pin41 = 40;
	static const unsigned short pin42 = 41;
	static const unsigned short pin43 = 42;
	static const unsigned short pin44 = 43;
	static const unsigned short pin45 = 44;
	static const unsigned short pin46 = 45;
	static const unsigned short pin47 = 46;
	static const unsigned short pin48 = 47;
	static const unsigned short pin49 = 48;
	static const unsigned short pin50 = 49;
	static const unsigned short pin51 = 50;
	static const unsigned short pin52 = 51;
	static const unsigned short pin53 = 52;
	static const unsigned short pin54 = 53;
#endif

	static void pin_write(unsigned short pin, int& value);
	static int pin_read(unsigned short pin);
	static void pin_out(unsigned short pin, int& value);
	static void pin_in(unsigned short pin);
	static int adc_read(unsigned short pin);

private:
	CPU(void) {}
};

#endif /* __STL_CPU_H__ */

