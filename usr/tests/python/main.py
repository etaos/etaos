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

import sys, cpu
from eeprom import EE24C02

cpu.set_output(15, False)

ee = EE24C02()

while True:
	cpu.write(15, True)
	sys.wait(500)
	cpu.write(15, False)
	sys.wait(500)

	ee.write(0x40, [155, 140, 120])
	data = ee.read(0x40, 3)
	print data

