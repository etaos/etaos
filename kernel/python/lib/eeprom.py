#
#   ETA/OS - EEPROM library
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

## @addtogroup python-eeprom
# @{

## @package EEPROM
#  @brief Provides PyMite's EEPROM module

import device
from device import Device

__name__ = "eeprom"

class EEPROM(Device):
	def __init__(self, name):
		Device.__init__(self, name)
		self.size = -1

	def write_string(self, addr, string):
		rv = self.write(string, addr)
		return rv

	def read_string(self, addr, length):
		rv = self.read(length, addr)
		return rv

	def read_float(self, addr):
		rv = self.read_numeral(addr, True)
		return rv

	def write_float(self, addr, num):
		rv = self.write_numeral(num, addr)
		return rv

## @}
