#
#   ETA/OS - TMP36 class
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

## @addtogroup python-lm36
# @{

## @package LM36
#  @brief Provides PyMite's LM36 sensor driver.

__name__ = "tmp36"

## LM36 class.
class TMP36:
	pin = 0

	## Create a new LM36 object.
	# @param pin Pin to which an LM36 device is connected.
	def __init__(self, pin):
		self.pin = pin

	## Read from the LM36 device.
	# @return The temperature as indicated by \p this device (in celcius).
	def read(self):
		t = cpu.analog_read(self.pin)
		t *= 5000.0 / 1024.0
		t -= 500.0
		return t / 10.0

## @}
