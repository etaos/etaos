#
#   ETA/OS - SRAM library
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

## @addtogroup python-sram
# @{

## @package SRAM
#  @brief Provides PyMite's SRAM drivers

__name__ = "sram"

## SRAM class
class SRAM(object):
	name = ""

	## Create a new SRAM object.
	# @param name Name of the SRAM device driver.
	def __init__(self, name):
		self.name = name

	## Read from \p this SRAM chip.
	# @param addr Address to start reading.
	# @param num Number of bytes to read.
	# @return An array list of the read bytes.
	def read(self, addr, num):
		return read(self.name, addr, num)

	## Write to \p this SRAM chip.
	# @param addr Address to start writing.
	# @param data List of bytes to write.
	# @param length Length of \p data.
	# @return None
	def write(self, addr, data, length):
		return write(self.name, addr, data, length)

	## Write a floating point number to \p this SRAM chip.
	# @param addr Address to write to.
	# @param flt Floating point number to write.
	# @return None
	def write_float(self, addr, flt):
		return write_float(self.name, addr, flt)

	## Read a floating point number.
	# @param addr Address to read from.
	# @return The float read from SRAM address \p addr.
	def read_float(self, addr):
		return read_float(self.name, addr)

	## Write to \p this SRAM chip.
	# @param addr Address to start writing.
	# @param string List of bytes to write.
	# @return None
	def write_string(self, addr, string):
		return write_string(self.name, addr, string, len(string))

	## Read from \p this SRAM chip.
	# @param addr Address to start reading.
	# @param length Number of bytes to read.
	# @return String object.
	def read_string(self, addr, length):
		return read_string(self.name, addr, length)

## @}

def write(name, addr, data, num):
	"""__NATIVE__
#ifdef CONFIG_PYTHON_SRAM
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, data, num, name;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	void *cdata;

	if (NATIVE_GET_NUM_ARGS() != 4) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	data = NATIVE_GET_LOCAL(2);
	num = NATIVE_GET_LOCAL(3);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(data) != OBJ_TYPE_LST ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val;
	cdata = kzalloc(cnum);

	list_to_byte_array(data, cdata, cnum);

	pm_sram_write(cname, caddr, cdata, cnum);
	kfree(cdata);
	NATIVE_SET_TOS(PM_NONE);

	return retval;
#else
	return PM_RET_OK;
#endif
	"""
	pass

def write_float(name, addr, flt):
	"""__NATIVE__
#ifdef CONFIG_PYTHON_SRAM
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, flt, name;
	const char *cname;
	uint16_t caddr;
	float cfloat;

	if (NATIVE_GET_NUM_ARGS() != 3) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	flt = NATIVE_GET_LOCAL(2);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(flt) != OBJ_TYPE_FLT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cfloat = ((pPmFloat_t)flt)->val;

	pm_sram_write_float(cname, caddr, cfloat);
	NATIVE_SET_TOS(PM_NONE);
	return retval;
#else
	return PM_RET_OK;
#endif
	"""
	pass

def read_float(name, addr):
	"""__NATIVE__
#ifdef CONFIG_PYTHON_SRAM
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, flt, name;
	const char *cname;
	uint16_t caddr;
	float cfloat;
	int rv;

	if (NATIVE_GET_NUM_ARGS() != 2) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;

	rv = pm_sram_read_float(cname, caddr, &cfloat);

	if(rv < 0) {
		NATIVE_SET_TOS(PM_NONE);
		return retval;
	}

	retval = float_new(cfloat, &flt);
	NATIVE_SET_TOS(flt);
	return retval;
#else
	return PM_RET_OK;
#endif
	"""
	pass

def read(name, addr, num):
	"""__NATIVE__
#ifdef CONFIG_PYTHON_SRAM
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, data, num, name;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	void *cdata;

	if (NATIVE_GET_NUM_ARGS() != 3) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	num = NATIVE_GET_LOCAL(2);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val;
	cdata = kzalloc(cnum);

	pm_sram_read(cname, caddr, cdata, cnum);
	data = list_create_from_ptr(cdata, cnum);
	kfree(cdata);

	NATIVE_SET_TOS(data);
	return retval;
#else
	return PM_RET_OK;
#endif
	"""
	pass

def write_string(name, addr, string, length):
	"""__NATIVE__
#ifdef CONFIG_PYTHON_SRAM
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, data, num, name;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	void *cdata;

	if (NATIVE_GET_NUM_ARGS() != 4) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	data = NATIVE_GET_LOCAL(2);
	num = NATIVE_GET_LOCAL(3);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(data) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val + 1;
	cdata = (void*)((pPmString_t)data)->val;

	pm_sram_write(cname, caddr, cdata, cnum);
	NATIVE_SET_TOS(PM_NONE);
	return retval;
#else
	return PM_RET_OK;
#endif
	"""
	pass

def read_string(name, addr, length):
	"""__NATIVE__
#ifdef CONFIG_PYTHON_SRAM
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t name, addr, num, text;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	char *cdata, *cpy;

	if (NATIVE_GET_NUM_ARGS() != 3) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	num = NATIVE_GET_LOCAL(2);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val + 1;
	cpy = cdata = kzalloc(cnum);

	pm_sram_read(cname, caddr, cdata, cnum - 1);

	string_new((uint8_t const **)&cdata, &text);
	kfree(cpy);

	NATIVE_SET_TOS(text);
	return retval;
#else
	return PM_RET_OK;
#endif
	"""
	pass
