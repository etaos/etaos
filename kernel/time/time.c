/*
 *  ETA/OS - Local time functions
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/time.h>
#include <etaos/tick.h>

/**
 * @brief Get the sytem time.
 * @param Storage pointer for the system time.
 * @return The system time.
 *
 * The \p time function returns the number of seconds elapsed since midnight
 * on the 1st of January 1970 (01-01-1970 00:00:00). The parameter \p now may
 * be NULL, in which case it is ignored.
 */
time_t time(time_t *now)
{
	time_t n;

	n = systick_get_seconds();
	if(now)
		now[0] = n;

	return n;
}

int localtime_r(const time_t *t, struct tm *time)
{
	return 0;
}

struct tm *localtime(const time_t *t)
{
	if(localtime_r(t, &_tm))
		return NULL;
	else
		return &_tm;
}

