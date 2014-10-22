/*
 *  ETA/OS - Platform driver
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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

extern int pgpio_write_pin(int pin, int val);
extern int pgpio_read_pin(int pin);
extern int pgpio_direction_output(int pin, int val);
extern int pgpio_direction_input(int pin);
extern int pgpio_pin_release(int pin);
extern int pgpio_pin_request(int pin);

#endif
