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
 * @param t Pointer to stored time.
 * @param tm_struct tm structure to store the converted time.
 * @return If an error occured, nonzero will be returned.
 *
 * Thread safe version of gmtime.
 */
int gmtime_r(const time_t *t, struct tm *tm_struct)
{
	int tmp;
	int *mdays;
	bool isleap = false;
	uint32_t tval;

	if(!tm_struct)
		return -EINVAL;

	tmp = *t / _YEAR_SEC; /* years since 1970 */
	tval = *t - (tmp * _YEAR_SEC); /* seconds within the current year */

	/* determine if we are in a leap year */
	tmp += 1970;
	if(((tmp % 4) == 0 && (tmp % 100) != 0) || (tmp % 400) == 0)
		isleap = true;

	tmp -= 1900;
	tm_struct->tm_year = tmp;
	tm_struct->tm_yday = tval / _DAY_SEC;

	tval -= tm_struct->tm_yday * _DAY_SEC; /* Number of seconds left */

	if(isleap)
		mdays = _lpdays;
	else
		mdays = _days;

	for(tmp = 1; mdays[tmp] < tm_struct->tm_yday; tmp++);
	tm_struct->tm_mon = --tmp;
	tm_struct->tm_mday = tm_struct->tm_yday - mdays[tmp];
	tm_struct->tm_wday = ((int) (*t / _DAY_SEC) + _BASE_DOW) % 7;
	tm_struct->tm_hour = (int) (tval / 3600);
	tval -= (time_t) tm_struct->tm_hour * 3600;

	tm_struct->tm_min = tval / 60;
	tval -= tm_struct->tm_min * 60;
	tm_struct->tm_sec = tval;
	tm_struct->tm_isdst = false;

	return 0;
}

/**
 * @brief Convert a time value.
 * @param t Pointer to stored time.
 * @return Returns a tm structure with the converted time. NULL is returned
 *         if any error occured.
 * @note This funciton is \b NOT thread safe.
 */
struct tm *gmtime(const time_t *t)
{
	if(gmtime_r(t, &_tm))
		return NULL;
	else
		return &_tm;
}

/** @} */

