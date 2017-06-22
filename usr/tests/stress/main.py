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

lm = LM35(0)
ee = EE24C02()
data_ary = [155, 120, 10, 20, 52, 80]
ee.write(0x60, data_ary, len(data_ary))

while True:
	data = ee.read(0x60, 2);
	print "EEPROM: %d and %d - Temperature: %f" % (data[0], data[1], lm.read())
	sys.wait(1000)

