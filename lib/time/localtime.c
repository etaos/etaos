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

/**
 * @addtogroup libctime
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/time.h>
#include <etaos/tick.h>

/**
 * @brief Timezone difference in seconds.
 */
long _timezone = -3600;
/**
 * @brief DST difference in seconds.
 */
long _dstbias = CONFIG_DST_BIAS;
/**
 * @brief Daylight savings control flag.
 */
bool _daylight = true;

/**
 * @brief Get the localtime, keeping the timezone and DST in mind.
 * @param t Pointer to stored time.
 * @param time Structure to store the converted time.
 * @return Nonzero in case of error.
 *
 * This is the thread safe version of localtime.
 */
int localtime_r(const time_t *t, struct tm *time)
{
	time_t timer;

	timer = *t - _timezone;
	gmtime_r(&timer, time);

	if(_daylight && time_isindst(time)) {
		timer -= CONFIG_DST_BIAS;
		gmtime_r(&timer, time);
		time->tm_isdst = true;
	}

	return 0;
}

/**
 * @brief Convert a epoch timer to a time structure.
 * @param t Pointer to stored time.
 * @return Converted time, or NULL in case of error.
 * @note This function is not thread safe.
 */
struct tm *localtime(const time_t *t)
{
	if(localtime_r(t, &_tm))
		return NULL;
	else
		return &_tm;
}

/** @} */

