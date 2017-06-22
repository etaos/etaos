#
#   ETA/OS - CPU class
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

__name__ = "cpu"

def write(pin, value):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t pin, value;
	int32_t _pin;
	bool _value;

	if (NATIVE_GET_NUM_ARGS() != 2) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	pin = NATIVE_GET_LOCAL(0);
	value = NATIVE_GET_LOCAL(1);

	if(OBJ_GET_TYPE(pin) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(value) != OBJ_TYPE_BOOL) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	_pin = ((pPmInt_t)pin)->val;
	_value = ((pPmBoolean_t)value)->val != 0;

	pm_cpu_pin_write(_pin, _value);
	return retval;
	"""
	pass

def set_output(pin, value):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t pin, value;
	int32_t _pin;
	bool _value;

	if (NATIVE_GET_NUM_ARGS() != 2) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	pin = NATIVE_GET_LOCAL(0);
	value = NATIVE_GET_LOCAL(1);

	if(OBJ_GET_TYPE(pin) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(value) != OBJ_TYPE_BOOL) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	_pin = ((pPmInt_t)pin)->val;
	_value = ((pPmBoolean_t)value)->val != 0;

	pm_cpu_direction_output(_pin, _value);
	return retval;
	"""
	pass

def set_input(pin):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t pin;
	int32_t _pin;

	if (NATIVE_GET_NUM_ARGS() != 1) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	pin = NATIVE_GET_LOCAL(0);

	if(OBJ_GET_TYPE(pin) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	_pin = ((pPmInt_t)pin)->val;

	pm_cpu_direction_input(_pin);
	return retval;
	"""
	pass

def read(pin):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t pin;
	int32_t _pin;
	pPmObj_t rv;

	if (NATIVE_GET_NUM_ARGS() != 1) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	pin = NATIVE_GET_LOCAL(0);

	if(OBJ_GET_TYPE(pin) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	_pin = ((pPmInt_t)pin)->val;
	rv = pm_cpu_pin_read(_pin) ? PM_TRUE : PM_FALSE;
	NATIVE_SET_TOS(rv);

	return retval;
	"""
	pass

def analog_read(pin):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t pin;
	int32_t _pin;
	pPmObj_t rv;
	float value;

	if (NATIVE_GET_NUM_ARGS() != 1) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	pin = NATIVE_GET_LOCAL(0);

	if(OBJ_GET_TYPE(pin) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	_pin = ((pPmInt_t)pin)->val;
	value = pm_cpu_analog_pin_read(_pin);
	retval = float_new(value, &rv);
	NATIVE_SET_TOS(rv);

	return retval;
	"""
	pass

