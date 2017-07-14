#
#   ETA/OS - Python time functions
#   Copyright (C) 2017  Michel Megens <dev@michelmegens.net>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

## @addtogroup python-time
# @{

## @package time
#  @brief Provides PyMite's time module, time
#
#  USAGE
#  -----
#
#  import time
#

"""__NATIVE__
#include <etaos/time.h>
"""

__name__ = "time"

## Time period object
class TimePeriod(object):
	_unused, SECONDS, MINUTES, HOURS, DAYS, MONTHS, YEARS = range(7)

## Time object
class Time(object):
	date = [0, 0, 0, 0, 0, 0]
	# 0: years
	# 1: months
	# 2: days
	# 3: hours
	# 4: mins
	# 5: seconds

	## Create a new time object.
	# @param init Initializes the new object to \p now.
	def __init__(self, init = False):
		if init:
			self.now()
		else:
			pass

	## Set the timestamp of \p this object to \p now.
	# @return None.
	def now(self):
		"""__NATIVE__
		time_t now;
		struct tm tm;
		pPmObj_t self, ary, val;
		PmReturn_t retval = PM_RET_OK;

		time(&now);
		localtime_r(&now, &tm);
		self = NATIVE_GET_LOCAL(0);

		if (OBJ_GET_TYPE(self) != OBJ_TYPE_CLI) {
			PM_RAISE(retval, PM_RET_EX_VAL);
			return retval;
		}

		class_get_attrib(self, "date", &ary);

		/* years */
		int_new(tm.tm_year + 1900, &val);
		list_setItem(ary, 0, val);

		/* months */
		int_new(tm.tm_mon + 1, &val);
		list_setItem(ary, 1, val);

		/* days */
		int_new(tm.tm_mday, &val);
		list_setItem(ary, 2, val);

		/* hours */
		int_new(tm.tm_hour, &val);
		list_setItem(ary, 3, val);

		/* minutes */
		int_new(tm.tm_min, &val);
		list_setItem(ary, 4, val);

		/* seconds */
		int_new(tm.tm_sec, &val);
		list_setItem(ary, 5, val);

		return retval;
		"""
		pass

	## Return the years since 1900.
	# @return Number of years since 1900.
	def years(self):
		return self.date[0]

	## Return the month of the year.
	# @return Month.
	def months(self):
		return self.date[1]

	## Return the day of the month.
	# @return Day of the month.
	def days(self):
		return self.date[2]

	## Return the hour of the day.
	# @return Hour.
	def hours(self):
		return self.date[3]

	## Return the minutes of the hour.
	# @return Minutes.
	def minutes(self):
		return self.date[4]

	## Return the seconds of the minute.
	# @return Seconds.
	def seconds(self):
		return self.date[5]

	## Return a (civilized) date format
	# (days/months/years hours:minutes:seconds)
	# @return String representation of \p this object.
	def to_string(self):
		return "%d/%d/%d %d:%d:%d" % (self.days(), self.months(),
				self.years(), self.hours(), self.minutes(),
				self.seconds())

## Get the current time stamp.
# @return The current timestamp in a list, with the least significant part of
#         timestamp in position 0.
def timestamp():
	"""__NATIVE__
	time_t now;
	uint32_t hi, lo;
	pPmObj_t pHi, pLo;
	pPmObj_t ary;

	time(&now);
	hi = now >> 32;
	lo = now & 0xFFFFFFFFL;
	int_new(hi, &pHi);
	int_new(lo, &pLo);
	list_new(&ary);
	list_append(ary, pHi);
	list_append(ary, pLo);

	NATIVE_SET_TOS(ary);
	return PM_RET_OK;
	"""
	pass

## @}

