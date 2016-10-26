/*
 *  ETA/OS - ETA/OS STL CPU
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

/**
 * @file include/etaos/stl/cpu.h STL CPU header
 */

#ifndef __STL_CPU_H__
#define __STL_CPU_H__

/**
 * @addtogroup stl
 * @{
 */

#include <etaos/types.h>
#include <etaos/gpio.h>
#include <etaos/analog.h>

#include <asm/io.h>

/**
 * @brief Utility class to provide access to hardware.
 *
 * The CPU class contains utility methods to access the core of the
 * hardware ETA/OS is running on. There is no need to create on instance
 * of CPU since all of its methods and fields are declared static.
 */
class CPU {
public:
#if GPIO_PINS >= 24
	static const unsigned short pin0 = 0; //!< GPIO pin 0 definition
	static const unsigned short pin1 = 1; //!< GPIO pin 1 definition
	static const unsigned short pin2 = 2; //!< GPIO pin 2 definition
	static const unsigned short pin3 = 3; //!< GPIO pin 3 definition
	static const unsigned short pin4 = 4; //!< GPIO pin 4 definition
	static const unsigned short pin5 = 5; //!< GPIO pin 5 definition
	static const unsigned short pin6 = 6; //!< GPIO pin 6 definition
	static const unsigned short pin7 = 7; //!< GPIO pin 7 definition
	static const unsigned short pin8 = 8; //!< GPIO pin 8 definition
	static const unsigned short pin9 = 9; //!< GPIO pin 9 definition
	static const unsigned short pin10 = 10; //!< GPIO pin 10 definition
	static const unsigned short pin11 = 11; //!< GPIO pin 11 definition
	static const unsigned short pin12 = 12; //!< GPIO pin 12 definition
	static const unsigned short pin13 = 13; //!< GPIO pin 13 definition
	static const unsigned short pin14 = 14; //!< GPIO pin 14 definition
	static const unsigned short pin15 = 15; //!< GPIO pin 15 definition
	static const unsigned short pin16 = 16; //!< GPIO pin 16 definition
	static const unsigned short pin17 = 17; //!< GPIO pin 17 definition
	static const unsigned short pin18 = 18; //!< GPIO pin 18 definition
	static const unsigned short pin19 = 19; //!< GPIO pin 19 definition
	static const unsigned short pin20 = 20; //!< GPIO pin 20 definition
	static const unsigned short pin21 = 21; //!< GPIO pin 21 definition
	static const unsigned short pin22 = 22; //!< GPIO pin 22 definition
	static const unsigned short pin23 = 23; //!< GPIO pin 23 definition
#endif
#if GPIO_PINS >= 54
	static const unsigned short pin24 = 24; //!< GPIO pin 24 definition
	static const unsigned short pin25 = 25; //!< GPIO pin 25 definition
	static const unsigned short pin26 = 26; //!< GPIO pin 26 definition
	static const unsigned short pin27 = 27; //!< GPIO pin 27 definition
	static const unsigned short pin28 = 28; //!< GPIO pin 28 definition
	static const unsigned short pin29 = 29; //!< GPIO pin 29 definition
	static const unsigned short pin30 = 30; //!< GPIO pin 30 definition
	static const unsigned short pin31 = 31; //!< GPIO pin 31 definition
	static const unsigned short pin32 = 32; //!< GPIO pin 32 definition
	static const unsigned short pin33 = 33; //!< GPIO pin 33 definition
	static const unsigned short pin34 = 34; //!< GPIO pin 34 definition
	static const unsigned short pin35 = 35; //!< GPIO pin 35 definition
	static const unsigned short pin36 = 36; //!< GPIO pin 36 definition
	static const unsigned short pin37 = 37; //!< GPIO pin 37 definition
	static const unsigned short pin38 = 38; //!< GPIO pin 38 definition
	static const unsigned short pin39 = 39; //!< GPIO pin 39 definition
	static const unsigned short pin40 = 40; //!< GPIO pin 40 definition
	static const unsigned short pin41 = 41; //!< GPIO pin 41 definition
	static const unsigned short pin42 = 42; //!< GPIO pin 42 definition
	static const unsigned short pin43 = 43; //!< GPIO pin 43 definition
	static const unsigned short pin44 = 44; //!< GPIO pin 44 definition
	static const unsigned short pin45 = 45; //!< GPIO pin 45 definition
	static const unsigned short pin46 = 46; //!< GPIO pin 46 definition
	static const unsigned short pin47 = 47; //!< GPIO pin 47 definition
	static const unsigned short pin48 = 48; //!< GPIO pin 48 definition
	static const unsigned short pin49 = 49; //!< GPIO pin 49 definition
	static const unsigned short pin50 = 50; //!< GPIO pin 50 definition
	static const unsigned short pin51 = 51; //!< GPIO pin 51 definition
	static const unsigned short pin52 = 52; //!< GPIO pin 52 definition
	static const unsigned short pin53 = 53; //!< GPIO pin 53 definition
#endif

	static void pin_write(unsigned short pin, int& value);
	static int pin_read(unsigned short pin);
	static void pin_out(unsigned short pin, int& value);
	static void pin_in(unsigned short pin);
	static int adc_read(unsigned short pin);

private:
	CPU(void) {}
};

/** @} */

#endif /* __STL_CPU_H__ */

