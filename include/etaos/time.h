/*
 *  ETA/OS - Timer header
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

/**
 * @file include/etaos/time.h Time handling
 */
/**
 * @addtogroup libctime
 * @{
 */

#ifndef __TIME_H__
#define __TIME_H__

#include <etaos/kernel.h>
#include <etaos/timer.h>
#include <etaos/spinlock.h>

/**
 * @brief Time structure.
 */
struct tm {
	int tm_sec, //!< Elapsed seconds.
	    tm_min, //!< Elapsed minutes.
	    tm_hour, //!< Hour of the day.
	    tm_mday, //!< Day of the month.
	    tm_mon, //!< Month of the year.
	    tm_year, //!< Year since 1900.
	    tm_wday, //!< Day of the week.
	    tm_yday, //!< Day of the year.
	    tm_isdst; //!< DST flag.

};

#define _BASE_DOW 4 //!< Base day of the week.
#define _YEAR_SEC (_DAY_SEC * 365UL) //!< Seconds per year.
#define _DAY_SEC (24UL * 60UL * 60UL) //!< Seconds per day.
#define _FOUR_YEAR_SEC (1461L * _DAY_SEC) //!< Seconds per four years.
#define _LEAP_YEAR_ADJUST 17L //!< Leap year adjustment.

extern struct tm _tm;
extern int _days[];
extern int _lpdays[];
extern bool _daylight;
extern long _timezone;
extern long _dstbias;

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

/** @} */

