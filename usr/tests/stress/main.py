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
from lm35 import LM35
from eeprom import EEPROM
from time import Time
from device import Device
from sram import SRAM

ee = EEPROM("24C02")
ram = SRAM("23K256")
sram_write_data = math.pi
addr = 0x60
data_ary = [155.0, 121.3, 3.1415, 12.2, 90.0, 8.91]

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
		ee.open()
		ee_data = ee.read_list(len(data_ary), True, addr)
		ee.close()

		if len(ee_data) is 0:
			continue

		vlength = math.hypot(2.0, 2.0)
		sram_data = ram.read_float(addr)
		if sram_data is not None:
			sram_data = math.sin(sram_data / 2.0)
		else:
			sram_data = 0.0

		print "[python]:    EEPROM: %f and %f" % (ee_data[4], ee_data[5])
		print "[python]:    SRAM: %f :: Length of (2, 2): %f" % (sram_data, vlength)
		sys.wait(1000)

def main():
	lm = LM35(0)
	ee.open()
	ee.write_list(data_ary, True, addr)
	ee.close()
	ram.write_float(addr, sram_write_data)

	# Setup the LED pin
	avr.port_direction_or(avr.portb, 0x80)
	sys.run(print_eeprom_and_sram)

	while True:
		print_temperature(lm)

main()
