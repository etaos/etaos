#
#   ETA/OS - LM35 class
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

## @addtogroup python-tmp35
# @{

## @package TMP35
#  @brief Provides PyMite's TMP35 sensor driver.

import cpu

__name__ = "lm35"

## LM35 class
class LM35:
	pin = 0

	## Create a new LM35 object.
	# @param pin Analog pin to which an LM35 device is connected.
	def __init__(self, pin):
		self.pin = pin

	## Read from \p this device.
	# @return the temperature value as indicated (in celcius).
	def read(self):
		t = cpu.analog_read(self.pin)
		t /= 1024;
		t *= 5000;
		return t / 10;

## @}

