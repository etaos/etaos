#!/usr/bin/env python

#
#   ETA/OS - List Python library files
#   Copyright (C) 2015  Michel Megens <dev@michelmegens.net>
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

import sys, getopt, os

class PyLibSearcher(object):
	dirs = ["kernel/python/lib", "arch/avr/kernel/lib"]
	results = []
	excludes = ["pmfeatures.py", "built-in.o", "Makefile"]

	def __init__(self, dirs):
		if len(dirs) != 0:
			self.dirs = dirs
	
	def search(self):
		for dirname in self.dirs:
			self.raw_search(dirname)

	def raw_search(self, dirpath):
		for root, subdirs, files in os.walk(dirpath):
			for subdir in subdirs:
				self.raw_search(subdir)

			for filename in files:
				path = "%s/%s" % (root, filename)
				if filename in self.excludes:
					continue

				if filename.endswith('.py'):
					self.results.append(path)
	
	def print_results(self):
		print " ".join(self.results)

def version():
	return "0.1.0"

def usage():
	print "USAGE: pylib-list.py [[--version] | [--help]] | [SEARCH DIRS]"

def parse_args():
	options, dirs = getopt.getopt(sys.argv[1:], 'vh', ['version', 'help'])

	for opt, arg in options:
		if opt in ('-v', '--version'):
			print "pylib-list %s" % version()
			sys.exit(0)
		if opt in ('-h', '--help'):
			usage()
			sys.exit(0)

	return dirs

def main():
	args = parse_args()
	if len(args) == 0:
		print "Warning: no search path given, using defaults"
	
	searcher = PyLibSearcher(args)
	searcher.search()
	searcher.print_results()

if __name__ == "__main__":
	main()
	sys.exit(0)

