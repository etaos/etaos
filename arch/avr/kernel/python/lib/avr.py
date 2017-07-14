#
#   ETA/OS - AVR library
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

## @addtogroup python-avr
# @{

## @package avr
#  @brief Provides PyMite's AVR module.
#
#  USAGE
#  -----
#
#  import avr
#

"""__NATIVE__
#include <asm/io.h>
"""

__name__ = "avr"

none, porta, portb, portc, portd, porte, portf, portg, porth, porti, \
		portj, portk, portl = range(13)

## Apply \p value with an OR operation to \p port.
# @param port I/O port.
# @param value Value to apply to \p port.
# @return None.
def port_or(port, value):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t port, value;
	uint8_t cport, cvalue;
	volatile unsigned char *port_ptr;

	if (NATIVE_GET_NUM_ARGS() != 2) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	port = NATIVE_GET_LOCAL(0);
	value = NATIVE_GET_LOCAL(1);

	if(OBJ_GET_TYPE(port) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(value) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cvalue = ((pPmInt_t)value)->val & 0xFF;
	cport  = ((pPmInt_t)port)->val & 0xFF;
	port_ptr = pm_port_to_addr(cport, PORT);
	
	irq_enter_critical();
	*port_ptr |= cvalue;
	irq_exit_critical();

	return retval;
	"""
	pass

## Apply \p value with an AND operation to \p port.
# @param port I/O port.
# @param value Value to apply to \p port.
# @return None.
def port_and(port, value):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t port, value;
	uint8_t cport, cvalue;
	volatile unsigned char *port_ptr;

	if (NATIVE_GET_NUM_ARGS() != 2) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	port = NATIVE_GET_LOCAL(0);
	value = NATIVE_GET_LOCAL(1);

	if(OBJ_GET_TYPE(port) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(value) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cvalue = ((pPmInt_t)value)->val & 0xFF;
	cport  = ((pPmInt_t)port)->val & 0xFF;
	port_ptr = pm_port_to_addr(cport, PORT);

	irq_enter_critical();
	*port_ptr &= cvalue;
	irq_exit_critical();

	return retval;
	"""
	pass

## Read from \p port.
# @param port I/O port.
# @return The value of \p port.
def port_read(port):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t port, value;
	uint8_t cvalue, cport;
	volatile unsigned char *port_ptr;

	if (NATIVE_GET_NUM_ARGS() != 1) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	port = NATIVE_GET_LOCAL(0);

	if(OBJ_GET_TYPE(port) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cport  = ((pPmInt_t)port)->val & 0xFF;
	port_ptr = pm_port_to_addr(cport, PIN);

	irq_enter_critical();
	cvalue = *port_ptr;
	irq_exit_critical();

	int_new(cvalue, &value);
	NATIVE_SET_TOS(value);

	return retval;
	"""
	pass

## Set the direction of \p port to value using an OR operation.
# @param port I/O port.
# @param value Value to apply to \p port.
# @return None.
def port_direction_or(port, value):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t port, value;
	uint8_t cport, cvalue;
	volatile unsigned char *port_ptr;

	if (NATIVE_GET_NUM_ARGS() != 2) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	port = NATIVE_GET_LOCAL(0);
	value = NATIVE_GET_LOCAL(1);

	if(OBJ_GET_TYPE(port) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(value) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cvalue = ((pPmInt_t)value)->val & 0xFF;
	cport  = ((pPmInt_t)port)->val & 0xFF;
	port_ptr = pm_port_to_addr(cport, DDR);

	irq_enter_critical();
	*port_ptr |= cvalue;
	irq_exit_critical();

	return retval;
	"""
	pass

## Set the direction of \p port to value using an AND operation.
# @param port I/O port.
# @param value Value to apply to \p port.
# @return None
def port_direction_and(port, value):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t port, value;
	uint8_t cport, cvalue;
	volatile unsigned char *port_ptr;

	if (NATIVE_GET_NUM_ARGS() != 2) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	port = NATIVE_GET_LOCAL(0);
	value = NATIVE_GET_LOCAL(1);

	if(OBJ_GET_TYPE(port) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(value) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cvalue = ((pPmInt_t)value)->val & 0xFF;
	cport  = ((pPmInt_t)port)->val & 0xFF;
	port_ptr = pm_port_to_addr(cport, DDR);

	irq_enter_critical();
	*port_ptr &= cvalue;
	irq_exit_critical();

	return retval;
	"""
	pass

## @}

