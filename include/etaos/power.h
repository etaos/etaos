/*
 *  ETA/OS - Power management
 *  Copyright (C) 2016   Michel Megens <dev@michelmegens.net>
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

#ifndef __POWER_H__
#define __POWER_H__

#include <etaos/kernel.h>

#define POWER_IDLE	0x1
#define POWER_STANDBY	0x2
#define POWER_SAVE	0x4
#define POWER_DOWN	0x8

extern void arch_hibernate(void);
extern void arch_set_power_mode(int mode);

static inline void power_set_mode(int mode)
{
	arch_set_power_mode(mode);
}

static inline void hibernate(void)
{
	arch_hibernate();
}

#endif /* __POWER_H__ */

