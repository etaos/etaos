#
#   ETA/OS - EEPROMIFY
#   Copyright (C) 2017  Michel Megens <dev@bietje.net>
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

import yaml, sys, getopt, os

def version():
	return "0.1.0"

def usage():
	print "USAGE: %s [[--version] | [--help]] -o <output> | [SEARCH DIRS]" % sys.argv[0]

def parse_args():
	options, yaml = getopt.getopt(sys.argv[1:], 'ho:v', ['help', 'output=', 'version'])
        output = None

	for opt, arg in options:
                if opt in ('-o', '--output'):
                        output = arg
		if opt in ('-v', '--version'):
			print "%s %s" % ("eepromify", version())
			sys.exit(0)
		if opt in ('-h', '--help'):
			usage()
			sys.exit(0)

        if output is None:
                usage()
                sys.exit(1)

        return {'input': yaml[0], 'output': output}

def generate_source_file(data):
        variables = []
        extern = []
        for key in data:
                es = 'extern '
                if(data[key]['type'] == 'string'):
                        data[key]['type'] = 'const char *'
                        data[key]['value'] = "\"%s\"" % data[key]['value']
                s = "%s __eeprom %s" % (data[key]['type'], key)
                es += s + ';'
                s += ' = ' + str(data[key]['value'])
                s += ';'
                variables.append(s)
                extern.append(es)

        header = """
/*
 * DO NOT EDIT - THIS IS A GENERATED FILE
 *
 * This file has been generated by eepromify. Changes you make
 * will be discarded the next time this file is generated.
 */

#include <etaos/kernel.h>
#include <etaos/types.h>

#include <asm/eeprom.h>

"""
        source = "%s" % header
        for line in variables:
                header += line
                header += '\n'

        for line in extern:
                source += line
                source += '\n'

        return [header, source]

def main():
        args = parse_args()
        inputfile = args['input']
        outputfile = args['output']

        with open(inputfile) as stream:
                data = yaml.load(stream)
                data = generate_source_file(data)

        with open(outputfile + ".h", 'w+') as stream:
                stream.write(data[1])
        with open(outputfile + ".c", 'w+') as stream:
                stream.write(data[0])

if __name__ == "__main__":
        main()
        sys.exit(0)