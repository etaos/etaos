/*
 *  ETA/OS - SRAM header
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

#ifndef __PYTHON_SRAM_H__
#define __PYTHON_SRAM_H__

#include <etaos/kernel.h>

CDECL
extern int pm_sram_write(const char *name, uint16_t addr,
			 const void *buff, size_t length);
extern int pm_sram_read(const char *name, uint16_t addr,
		void *buff, size_t length);
extern int pm_sram_write_float(const char *name, uint16_t addr, float flt);
extern int pm_sram_read_float(const char *name, uint16_t addr, float *flt);
CDECL_END

#endif
