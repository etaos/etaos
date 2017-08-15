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

import sys, cpu, string, avr
from lm35 import LM35
from eeprom import EEPROM
from sram import SRAM
from time import Time

ee = EEPROM("24C02")
ram = SRAM("23K256")
sram_write_data = 2.718280
addr = 0x60
num = 2

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
		sram_data = ram.read_float(addr)
		ee_data = ee.read(addr, num)
		print "[python]:    EEPROM: %d and %d" % (ee_data[0], ee_data[1])
		print "[python]:    SRAM: %f" % sram_data
		sys.wait(1000)

def main():
	lm = LM35(0)
	data_ary = [155, 120, 10, 20, 52, 80]
	ee.write(addr, data_ary, len(data_ary))
	ram.write_float(addr, sram_write_data)

	# Setup the LED pin
	avr.port_direction_or(avr.portb, 0x80)
	sys.run(print_eeprom_and_sram)

	while True:
		print_temperature(lm)

main()

