/*
 *  ETA/OS - Spinlocks
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/spinlock.h>
#include <etaos/thread.h>
#include <etaos/stdio.h>

/**
 * @ingroup archAPI
 * @brief Raw version of spin_lock.
 * @param spin Spin lock to lock.
 */
#ifdef CONFIG_SPINLOCK_DEBUG
void arch_spin_lock(spinlock_t *spin, const char *file, int line)
#else
void arch_spin_lock(spinlock_t *spin)
#endif
{
#ifdef CONFIG_SPINLOCK_DEBUG
	struct thread * current = current_thread();
	if(spin->lock) {
		printf("DEADLOCK at %s:%i by %s\nPreviously acquired from: %s:%i by %s\n",
				file, line, current->name,
				spin->prev_file, spin->prev_line, spin->prev->name);
	}
#endif
	avr_spin_lock((unsigned char*)&spin->lock);
#ifdef CONFIG_SPINLOCK_DEBUG
	spin->prev_line = line;
	spin->prev_file = (char*)file;
	spin->prev = current;
#endif
}

/**
 * @ingroup archAPI
 * @brief Raw version of spin_unlock
 * @param spin Spin lock to unlock.
 */
#ifdef CONFIG_SPINLOCK_DEBUG
void arch_spin_unlock(spinlock_t *spin, const char *file, int line)
#else
void arch_spin_unlock(spinlock_t *spin)
#endif
{
#ifdef CONFIG_SPINLOCK_DEBUG
	spin->prev_line = 0;
	spin->prev_file = NULL;
	spin->prev = NULL;
#endif
	avr_spin_unlock((unsigned char*)&spin->lock);
}

