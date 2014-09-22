/*
 *  ETA/OS - Spinlock header
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

#ifndef __AVR_SPINLOCK_H__
#define __AVR_SPINLOCK_H__

#include <etaos/mutex.h>

extern void avr_spin_lock(unsigned char *);
extern void avr_spin_unlock(unsigned char*);

#define arch_mutex_lock(__l) arch_spin_lock(__l)
#define arch_mutex_unlock(__l) arch_spin_unlock(__l)

static inline void arch_spin_lock(mutex_t *mutex)
{
	avr_spin_lock((unsigned char*)&mutex->lock);
}

static inline void arch_spin_unlock(mutex_t *mutex)
{
	avr_spin_unlock((unsigned char*)&mutex->lock);
}
#endif
