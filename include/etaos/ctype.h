/*
 *  ETA/OS - LibC ctype
 *  Copyright (C) 2016  Michel Megens <dev@bietje.net>
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

#ifndef __CTYPE_H_
#define __CTYPE_H_

extern int isalpha(int c);
extern int isupper(int c);
extern int islower(int c);
extern int isdigit(int c);
extern int isxdigit(int c);
extern int isspace(int c);

#endif /* __CTYPE_H_ */

