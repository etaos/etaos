/*
 *  ETA/OS - General time library.
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

struct transitiondate {
	int yr,
	    yd;
	long ms;
};

#define DAY_MILLISEC    (24L * 60L * 60L * 1000L)

static struct transitiondate dststart = { -1, 0, 0L };
static struct transitiondate dstend = { -1, 0, 0L };

static inline bool is_leap_year(int year)
{
	year += 1900;

	if(((year & 3) == 0 && (year % 100) != 0) || (year % 400) == 0)
		return true;

	return false;
}

static void cvtdate(int trantype, int datetype, int year, int month, int week,
		int dayofweek, int date, int hour, int min, int sec, int msec)
{
	int yearday;
	int monthdow;

	if (datetype == 1) {
		/*
		 * Transition day specified in day-in-month format.
		 */

		/*
		 * Figure the year-day of the start of the month.
		 */
		yearday = 1 + (is_leap_year(year) ?
				_lpdays[month - 1] : _days[month - 1]);

		/*
		 * Figure the day of the week of the start of the month.
		 */
		monthdow = (yearday + ((year - 70) * 365) + ((year - 1) >> 2) -
			       	_LEAP_YEAR_ADJUST + _BASE_DOW) % 7;

		/*
		 * Figure the year-day of the transition date
		 */
		if (monthdow <= dayofweek)
			yearday += (dayofweek - monthdow) + (week - 1) * 7;
		else
			yearday += (dayofweek - monthdow) + week * 7;

		/*
		 * May have to adjust the calculation above if week == 5 (meaning
		 * the last instance of the day in the month). Check if year falls
		 * beyond after month and adjust accordingly.
		 */
		if ((week == 5) && (yearday > (is_leap_year(year) ?
					_lpdays[month] : _days[month]))) {
			yearday -= 7;
		}
	} else {
		/*
		 * Transition day specified as an absolute day
		 */
		yearday = is_leap_year(year) ? _lpdays[month - 1] : _days[month - 1];

		yearday += date;
	}

	if (trantype == 1) {
		/*
		 * Converted date was for the start of DST
		 */
		dststart.yd = yearday;
		dststart.ms = (long) msec + (1000L * (sec + 60L * (min + 60L * hour)));
		/*
		 * Set year field of dststart so that unnecessary calls to
		 * cvtdate() may be avoided.
		 */
		dststart.yr = year;
	} else {
		/*
		 * Converted date was for the end of DST
		 */
		dstend.yd = yearday;
		dstend.ms = (long) msec + (1000L * (sec + 60L * (min + 60L * hour)));
		/*
		 * The converted date is still a DST date. Must convert to a
		 * standard (local) date while being careful the millisecond field
		 * does not overflow or underflow.
		 */
		if ((dstend.ms += (CONFIG_DST_BIAS * 1000L)) < 0) {
		    dstend.ms += DAY_MILLISEC;
		    dstend.yd--;
		} else if (dstend.ms >= DAY_MILLISEC) {
		    dstend.ms -= DAY_MILLISEC;
		    dstend.yd++;
		}

		/*
		 * Set year field of dstend so that unnecessary calls to cvtdate()
		 * may be avoided.
		 */
		dstend.yr = year;
	}

	return;
}

int time_isindst(struct tm * tb)
{
	long ms;

	if (_daylight == 0)
		return 0;

	/*
	* Compute (recompute) the transition dates for daylight saving time
	* if necessary. The yr (year) fields of dststart and dstend is
	* compared to the year of interest to determine necessity.
	*/
	if ((tb->tm_year != dststart.yr) || (tb->tm_year != dstend.yr)) {
		cvtdate(1, 1, tb->tm_year, 3,   /* March */
			5,              /* last... */
			0,              /* ...Sunday */
			0, 2,           /* 02:00 (2 AM) */
			0, 0, 0);

		cvtdate(0, 1, tb->tm_year, 10,  /* October */
			5,              /* last... */
			0,              /* ...Sunday */
			0, 2,           /* 02:00 (2 AM) */
			0, 0, 0);
	}

	/*
	* Handle simple cases first.
	*/
	if (dststart.yd < dstend.yd) {
		/*
		 * Northern hemisphere ordering
		 */
		if ((tb->tm_yday < dststart.yd) || (tb->tm_yday > dstend.yd))
			return 0;
		if ((tb->tm_yday > dststart.yd) && (tb->tm_yday < dstend.yd))
			return 1;
	} else {
	/*
	 * Southern hemisphere ordering
	 */
	if ((tb->tm_yday < dstend.yd) || (tb->tm_yday > dststart.yd))
		return 1;
	if ((tb->tm_yday > dstend.yd) && (tb->tm_yday < dststart.yd))
		return 0;
	}

	ms = 1000L * (tb->tm_sec + 60L * tb->tm_min + 3600L * tb->tm_hour);

	if (tb->tm_yday == dststart.yd) {
		if (ms >= dststart.ms)
			return 1;
		else
			return 0;
	} else {
		if (ms < dstend.ms)
			return 1;
		else
			return 0;
	}
}

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

