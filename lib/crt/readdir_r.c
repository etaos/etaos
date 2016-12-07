/*
 *  ETA/OS - LibC readdir_r
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

/**
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/dirent.h>
#include <etaos/vfs.h>
#include <etaos/error.h>

/**
 * @brief Read from a directory stream.
 * @param dir Directory stream.
 * @param entry Previous entry
 * @param result Pointer to store the result (next entry).
 * @return The next directory entry or NULL.
 */
struct dirent *readdir_r(struct dirent *dir, struct dirent *entry,
		struct dirent **result)
{
	struct dirent *rv, *tmp;
	struct list_head *carriage;
	unsigned long flags;
	bool found = false;

	tmp = NULL;
	rv = NULL;

	spin_lock_irqsave(&vfs_lock, flags);
	list_for_each(carriage, &dir->children) {
		tmp = list_entry(carriage, struct dirent, entry);

		if(found || entry == NULL) {
			rv = tmp;
			break;
		}

		if(tmp == entry)
			found = true;
	}
	spin_unlock_irqrestore(&vfs_lock, flags);

	*result = rv;
	return rv;
}

/** @} */


