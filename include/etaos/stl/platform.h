/*
 *  ETA/OS - Platform header
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

#ifndef __PLATFORM_STL_H__
#define __PLATFORM_STL_H__

class Platform {
public:
	static void pin_write(unsigned short pin, int& value);
	static int pin_read(unsigned short pin);

	static void pin_out(unsigned short pin, int& value);
	static void pin_in(unsigned short pin);

private:
	explicit Platform() {}
};

#endif /* __PLATFORM_STL_H__ */

