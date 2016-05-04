/*
 *  ETA/OS - GMT time functions
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

int gmtime_r(const time_t *t, struct tm *gmt)
{
	return 0;
}

struct tm *gmtime(const time_t *t)
{
	if(gmtime_r(t, &_tm))
		return NULL;
	else
		return &_tm;
}

