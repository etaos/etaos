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

import sys, sram
from sram import SRAM
from eeprom import EEPROM

ram = SRAM("23K256")
ee = EEPROM("24C02")

data_ary = [100, 50, 33]
sram_str = "Some piece of text"
sram_len = len(sram_str)

ram.write_string(0x40, sram_str)
ee.write(0x60, data_ary, len(data_ary))

print "SRAM read: %s" % ram.read_string(0x40, sram_len)
print "EEPROM read:"
print ee.read(0x60, len(data_ary))
print "calypso_exit"

while True:
	pass

