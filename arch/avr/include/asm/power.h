/*
 *  ETA/OS - Power management
 *  Copyright (C) 2014, 2015, 2016   Michel Megens <dev@michelmegens.net>
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

#ifndef __AVR_POWER_H__
#define __AVR_POWER_H__

extern void avr_hibernate(void);
extern void avr_set_power_mode(int mode);

#define arch_power_set_mode(__x) avr_set_power_mode(__x)
#define arch_hibernate() avr_hibernate()

#endif /* __AVR_POWER_H__ */

