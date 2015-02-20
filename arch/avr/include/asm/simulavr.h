/*
 *  ETA/OS - SIMUL AVR
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

#ifndef __SIMUL_AVR_H__
#define __SIMUL_AVR_H__

#include <etaos/stdio.h>

CDECL
extern int simul_avr_write_byte(int c, struct vfile * stream);
extern void simul_avr_write_string(char *s, struct vfile * stream);
extern void simul_avr_setup_streams(void);
extern void simul_avr_exit(char err);
CDECL_END
#endif
