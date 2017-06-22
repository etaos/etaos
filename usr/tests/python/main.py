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

while True:
	print "SRAM read:"
	print ram.read_string(0x40, sram_len)
	sys.wait(1000)

