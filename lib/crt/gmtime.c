/*
 *  ETA/OS - GMT time functions
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
 * @addtogroup libctime
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/error.h>

int _days[] = {
	-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
};

int _lpdays[] = {
	-1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

struct tm _tm;

/**
 * @brief Convert a time value.
 * @param timer Pointer to stored time.
 * @param ptm tm structure to store the converted time.
 * @return If an error occured, nonzero will be returned.
 *
 * Thread safe version of gmtime.
 */
int gmtime_r(const time_t * timer, struct tm* ptm)
{
	time_t ctimer = *timer;     /* var to calculate with */
	bool isleapyear = false;
	uint32_t tmptimer;
	int *mdays;

	if(!ptm)            /* check pointer */
		return -EINVAL;

	/*
	First calculate the number of four-year-interval, so calculation
	of leap year will be simple. Btw, because 2000 IS a leap year and
	2100 is out of range, this formula is so simple.
	*/
	tmptimer = (ctimer / _FOUR_YEAR_SEC);
	ctimer -= (tmptimer * _FOUR_YEAR_SEC);

	/* Determine the correct year within the interval */
	tmptimer = (tmptimer * 4) + 70;     /* 1970, 1974, 1978,... */
	if (ctimer >= _YEAR_SEC) {
		tmptimer++;             /* 1971, 1975, 1979,... */
		ctimer -= _YEAR_SEC;
		if (ctimer >= _YEAR_SEC) {
			tmptimer++; /* 1972, 1976, 1980,... (all leap years!) */
			ctimer -= _YEAR_SEC;
			/* A leap year has 366 days, so compare t
			 _YEAR_SEC + _DAY_SEC */
			if (ctimer >= (_YEAR_SEC + _DAY_SEC)) {
				tmptimer++; /* 1973, 1977, 1981,... */
				ctimer -= (_YEAR_SEC + _DAY_SEC);
			} else {
				isleapyear = 1; /*If leap year, set the flag */
			}
		}
	}

	/*
	tmptimer now has the value for tm_year. ctimer now holds the
	number of elapsed seconds since the beginning of that year.
	*/
	ptm->tm_year = tmptimer;

	/*
	Calculate days since January 1st and store it to tm_yday.
	Leave ctimer with number of elapsed seconds in that day.
	*/
	ptm->tm_yday = (int) (ctimer / _DAY_SEC);
	ctimer -= (ptm->tm_yday) * _DAY_SEC;

	/*
	Determine months since January (Note, range is 0 - 11)
	and day of month (range: 1 - 31)
	*/
	if (isleapyear)
		mdays = _lpdays;
	else
		mdays = _days;


	for (tmptimer = 1; mdays[tmptimer] < ptm->tm_yday; tmptimer++);

	ptm->tm_mon = --tmptimer;

	ptm->tm_mday = ptm->tm_yday - mdays[tmptimer];

	/* Calculate day of week. Sunday is 0 */
	ptm->tm_wday = ((*timer / _DAY_SEC) + _BASE_DOW) % 7;

	/* Calculate the time of day from the remaining seconds */
	ptm->tm_hour = (ctimer / 3600);
	ctimer -= ptm->tm_hour * 3600L;

	ptm->tm_min = ctimer / 60;
	ptm->tm_sec = ctimer - (ptm->tm_min) * 60;
	ptm->tm_isdst = false;

	return 0;
}

/**
 * @brief Convert a time value.
 * @param t Pointer to stored time.
 * @return Returns a tm structure with the converted time. NULL is returned
 *         if any error occured.
 * @note This function is \b NOT thread safe.
 */
struct tm *gmtime(const time_t *t)
{
	if(gmtime_r(t, &_tm))
		return NULL;
	else
		return &_tm;
}

/** @} */
