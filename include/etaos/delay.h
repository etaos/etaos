/*
 *  ETA/OS - Delay header
 *  Copyright (C) 2014   Michel Megens
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

/** @file etaos/delay.h */

#ifndef __DELAY_H__
#define __DELAY_H__

/**
 * @ingroup archAPI
 * @brief Arch backend for micro second delays.
 * @param us Amount of micro seconds to delay.
 */
extern void arch_delay_us(double us);
#ifdef CONFIG_DELAY_US
extern void delay_us(double us);
#endif
#ifdef CONFIG_DELAY_MS
extern void delay(unsigned int ms);
#endif

#endif
