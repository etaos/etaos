/*
 *  ETA/OS - mktime
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
 * @addtogroup libctime
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/time.h>

#define check_add(dest, src1, src2)   ( ((src1 >= 0L) && (src2 >= 0L) \
    && (dest < 0L)) || ((src1 < 0L) && (src2 < 0L) && (dest >= 0L)) )
#define check_mul(dest, src1, src2)   ( src1 ? (dest/src1 != src2) : 0 )

#define BASE_YEAR 1900
#define EPOCH_BASE 1970

static inline bool is_leap_year(int year)
{
	year += 1900;

	if(((year & 3) == 0 && (year % 100) != 0) || (year % 400) == 0)
		return true;

	return false;
}

static time_t __mktime(struct tm *t, bool tz)
{
	time_t tmp1, tmp2, tmp3;

	if(!t)
		return 0;

	tmp1 = t->tm_year;
	if(t->tm_mon < 0 || t->tm_mon > 11) {
		tmp1 += t->tm_mon / 12;
		if((t->tm_mon %= 12) < 0) {
			t->tm_mon += 12;
			tmp1--;
		}
	}

	/* Calculate days elapsed minus one and adjust for leap years */
	tmp2 = _days[t->tm_mon];
	if(is_leap_year(tmp1) && t->tm_mon > 1)
		tmp2++;

	/* calculate the days since base midnight */
	tmp3 = ((tmp1 + BASE_YEAR - EPOCH_BASE) * 365ULL) + (
			((tmp1 - 1ULL) >> 2) - _LEAP_YEAR_ADJUST);
	tmp3 += tmp2;
	tmp1 = tmp3 + t->tm_mday;

	/* hours since base */
	tmp2 = tmp1 * 24ULL;
	tmp1 = tmp2 + t->tm_hour;

	/* minutes since base */
	tmp2 = tmp1 * 60ULL;
	tmp1 = tmp2 + t->tm_min;

	/* seconds since base */
	tmp2 = tmp1 * 60ULL;
	tmp1 = tmp2 + t->tm_sec;

	if(tz) {
		tmp1 += _timezone;

		/* adjust for DST */
		if(t->tm_isdst > 0)
			tmp1 += _dstbias;
	}

	return tmp1;
}

/**
 * @brief Convert a time structure to a epoch timer.
 * @param tm Time structure to convert.
 * @return The converted timer value.
 *
 * This function assumes that the time stored in \p tm is the localtime.
 */
time_t mktime(struct tm *tm)
{
	return __mktime(tm, true);
}

/**
 * @brief Convert a time structure to a epoch timer.
 * @param tm Time structure to convert.
 * @return The converted timer value.
 *
 * This function assumes that the time stored in \p tm is standard time (GMT).
 */
time_t _mktime(struct tm *tm)
{
	return __mktime(tm, false);
}

/** @} */

