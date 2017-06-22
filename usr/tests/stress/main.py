# This file is Copyright 2010 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
# 
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING up one directory from this.

import sys, cpu, string
from lm35 import LM35
from eeprom import EE24C02
from time import Time

chip = EE24C02()
addr = 0x60
num = 2

def print_temperature(sensor):
	tm = Time(True)
	while True:
		temp = sensor.read()
		tm.now()
		print "[python]:    Temperature: %f at %s" % (temp, tm.to_string())
		sys.wait(1000)

def print_eeprom():
	while True:
		data = chip.read(addr, num)
		print "[python]:    EEPROM: %d and %d" % (data[0], data[1])
		sys.wait(1000)

def main():
	lm = LM35(0)
	data_ary = [155, 120, 10, 20, 52, 80]
	chip.write(0x60, data_ary, len(data_ary))
	sys.run(print_eeprom)

	while True:
		print_temperature(lm)

main()

