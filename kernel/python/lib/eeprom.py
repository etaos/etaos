#
#   ETA/OS - I2C library
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

__name__ = "eeprom"

class EE24C02(object):
	def read(self, addr, num):
		"""__NATIVE__
		PmReturn_t retval = PM_RET_OK;
		pPmObj_t addr, num, list;
		uint8_t rawaddr;
		size_t rawnum;
		void *dst;

		if (NATIVE_GET_NUM_ARGS() != 3) {
			PM_RAISE(retval, PM_RET_EX_TYPE);
			return retval;
		}

		addr = NATIVE_GET_LOCAL(1);
		num = NATIVE_GET_LOCAL(2);

		if(OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
			OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
			PM_RAISE(retval, PM_RET_EX_TYPE);
			return retval;
		}

		rawaddr = ((pPmInt_t)addr)->val & 0xFF;
		rawnum  = ((pPmInt_t)num)->val;
		dst = kzalloc(rawnum);
		pm_ee_read(rawaddr, dst, rawnum);
		list = list_create_from_ptr(dst, rawnum);
		kfree(dst);
		NATIVE_SET_TOS(list);

		return retval;
		"""
		pass

	def write_byte(self, addr, byte):
		"""__NATIVE__
		PmReturn_t retval = PM_RET_OK;
		pPmObj_t addr, data;
		uint8_t rawaddr, rawdata;

		if (NATIVE_GET_NUM_ARGS() != 3) {
			PM_RAISE(retval, PM_RET_EX_TYPE);
			return retval;
		}

		addr = NATIVE_GET_LOCAL(1);
		data = NATIVE_GET_LOCAL(2);

		if(OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
			OBJ_GET_TYPE(data) != OBJ_TYPE_INT) {
			PM_RAISE(retval, PM_RET_EX_TYPE);
			return retval;
		}

		rawaddr = ((pPmInt_t)addr)->val & 0xFF;
		rawdata = ((pPmInt_t)data)->val & 0xFF;
		pm_ee_write(rawaddr, (void*)&rawdata, 1);

		return retval;
		"""
		pass

	def write(self, addr, data):
		for byte in data:
			self.write_byte(addr, byte)
			addr += 1

