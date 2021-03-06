/*
 *  ETA/OS - Python header
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
 *  Copyright (C) 2017   Dean Hall
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __INT_H__
#define __INT_H__

/**
 * \file
 * \brief Integer Object Type
 *
 * Integer object type header.
 */

/**
 * Integer obj
 *
 * 32b signed integer
 */
typedef struct PmInt_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Integer value */
	int32_t val;
} PmInt_t, *pPmInt_t;

/**
 * Creates a duplicate Integer object
 *
 * Created specifically for the index value in FOR_LOOP.
 *
 * @param   pint Pointer to int obj to duplicate.
 * @param   r_pint Return by ref, ptr to new int
 * @return  Return status
 */
PmReturn_t int_dup(pPmObj_t pint, pPmObj_t * r_pint);

/**
 * Creates a new Integer object
 *
 * @param   val Value to assign int (signed 32-bit).
 * @param   r_pint Return by ref, ptr to new int
 * @return  Return status
 */
PmReturn_t int_new(int32_t val, pPmObj_t * r_pint);

/**
 * Implements the UNARY_POSITIVE bcode.
 *
 * Creates a new int with the same value as the given int.
 *
 * @param   pobj Pointer to integer object
 * @param   r_pint Return by reference, ptr to int
 * @return  Return status
 */
PmReturn_t int_positive(pPmObj_t pobj, pPmObj_t * r_pint);

/**
 * Implements the UNARY_NEGATIVE bcode.
 *
 * Creates a new int with a value that is the negative of the given int.
 *
 * @param   pobj Pointer to target object
 * @param   r_pint Return by ref, ptr to int
 * @return  Return status
 */
PmReturn_t int_negative(pPmObj_t pobj, pPmObj_t * r_pint);

/**
 * Implements the UNARY_INVERT bcode.
 *
 * Creates a new int with a value that is
 * the bitwise inversion of the given int.
 *
 * @param   pobj Pointer to integer to invert
 * @param   r_pint Return by reference; new integer
 * @return  Return status
 */
PmReturn_t int_bitInvert(pPmObj_t pobj, pPmObj_t * r_pint);

#ifdef HAVE_PRINT
/**
 * Sends out an integer object in decimal notation with MSB first.
 * The number is preceded with a "-" when necessary.
 *
 * @param pObj Ptr to int object
 * @return Return status
 */
PmReturn_t int_print(pPmObj_t pint);

/**
 * Prints the Int object in ascii-coded hexadecimal out the platform output
 *
 * @param pint Pointer to Int object
 */
PmReturn_t int_printHex(pPmObj_t pint);
#endif				/* HAVE_PRINT */

/**
 * Returns by reference an integer that is x raised to the power of y.
 *
 * @param px The integer base
 * @param py The integer exponent
 * @param r_pn Return by reference; New integer with value of x ** y
 * @return Return status
 */
PmReturn_t int_pow(pPmObj_t px, pPmObj_t py, pPmObj_t * r_pn);

/**
 * Returns by reference the result of the selected operation.
 *
 * @param px The integer numerator
 * @param py The integer denominator
 * @param op The operator selector.  '/' selects division, all else is modulus.
 * @param r_pn Return by reference; New integer with value of x / y or x % y.
 * @return Return status
 */
PmReturn_t int_divmod(pPmObj_t px, pPmObj_t py, uint8_t op, pPmObj_t * r_pxopy);

#endif				/* __INT_H__ */
