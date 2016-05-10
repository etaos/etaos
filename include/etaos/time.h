/*
 *  ETA/OS - Timer header
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

#ifndef __TIME_H__
#define __TIME_H__

#include <etaos/kernel.h>
#include <etaos/timer.h>
#include <etaos/spinlock.h>

struct tm {
	int tm_sec,
	    tm_min,
	    tm_hour,
	    tm_mday,
	    tm_mon,
	    tm_year,
	    tm_wday,
	    tm_yday,
	    tm_isdst;

};

#define _BASE_DOW 4
#define _YEAR_SEC (_DAY_SEC * 365UL)
#define _DAY_SEC (24UL * 60UL * 60UL)
#define _FOUR_YEAR_SEC (1461L * _DAY_SEC)
#define _LEAP_YEAR_ADJUST 17L

extern struct tm _tm;
extern int _days[];
extern int _lpdays[];
extern bool _daylight;

CDECL
extern int stime(time_t time);
extern time_t time(time_t *now);
extern int time_isindst(struct tm * tb);

extern struct tm *localtime(const time_t *t);
extern int localtime_r(const time_t *t, struct tm *time);

extern struct tm *gmtime(const time_t *t);
extern int gmtime_r(const time_t *t, struct tm *gmt);

extern time_t mktime(struct tm *tm);
extern time_t _mktime(struct tm *tm);
CDECL_END

#endif

