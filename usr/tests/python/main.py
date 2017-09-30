#
#   ETA/OS - Python main
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

import sys, math

from sram import SRAM
from eeprom import EEPROM
from device import Device

class A(object):
	def __init__(self):
		self.name = "Hello"

def change_name(obj, name):
	"""__NATIVE__
	pPmObj_t obj, name;

	obj = NATIVE_GET_LOCAL(0);
	name = NATIVE_GET_LOCAL(1);

	return class_set_attr_cstr(obj, \"name\", name);
	"""
	pass

ram = SRAM("23K256")
ee = EEPROM("24C02")
aobj = A()

data_ary = [100.1, 50.2, 33.3]
data_len = len(data_ary)
sram_str = "Some piece of text"
sram_len = len(sram_str)

ram.write_string(0x40, sram_str)
ee.open()
ee.write_list(data_ary, True, 0x10)
ee.close()

change_name(aobj, "Hello, World")
print aobj.name
print math.atan2(1, 0)

ee.open()
readback = ee.read_list(data_len, True, 0x10)
ee.close()

print "EEPROM read:"
print "SRAM read: %s" % ram.read_string(0x40, sram_len)

print readback
print "calypso_exit"

while True:
	pass
