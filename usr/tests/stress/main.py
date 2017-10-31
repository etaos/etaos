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

import sys, cpu, string, avr, math
from tmp36 import TMP36
from eeprom import EEPROM
from time import Time
from device import Device
from sram import SRAM

__name__ = "__main__"

ee = EEPROM("24C02")
ram = SRAM("23K256")
addr = 0x60
data_ary = [155.0, 121.3, 3.1415, 12.2, 90.0, 8.91]
ram_lst = [3.1415, 12.1, math.pi]

def print_temperature(sensor):
	tm = Time(True)
	high = False
	while True:
		temp = sensor.read()
		tm.now()
		print "[python]:    Temperature: %f at %s" % (temp, tm.to_string())

		if high:
			avr.port_and(avr.portb, 0x7F)
			high = False
		else:
			avr.port_or(avr.portb, 0x80)
			high = True

		sys.wait(1000)

def print_eeprom_and_sram():
	while True:
		sram_data = ram.read_list(len(ram_lst), True, addr)
		ee_data = ee.read_list(len(data_ary), True, addr)

		if len(ee_data) is 0:
			continue

		vlength = math.hypot(2.0, 2.0)
		if len(sram_data) is not 0:
			sram_data = math.sin(sram_data[2] / 2.0)
		else:
			sram_data = 0.0

		print "[python]:    EEPROM: %f and %f" % (ee_data[4], ee_data[5])
		print "[python]:    SRAM: %f :: Length of (2, 2): %f" % (sram_data, vlength)
		sys.wait(1000)

def main():
	lm = TMP36(0)
	ee.write_list(data_ary, True, addr)
	ram.write_list(ram_lst, True, addr)

	# Setup the LED pin
	avr.port_direction_or(avr.portb, 0x80)
	sys.run(print_eeprom_and_sram)

	while True:
		print_temperature(lm)

main()
