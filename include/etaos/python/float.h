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

#ifndef __FLOAT_H__
#define __FLOAT_H__

/**
 * \file
 * \brief Float Object Type
 *
 * Float object type header.
 */

/**
 * Float obj
 *
 * 32b floating point number
 */
typedef struct PmFloat_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Float value */
	float val;
} PmFloat_t, *pPmFloat_t;

#ifdef HAVE_FLOAT

/**
 * Creates a new Float object
 *
 * @param   f Value to assign float (signed 32-bit).
 * @param   r_pint Return by ref, ptr to new float
 * @return  Return status
 */
PmReturn_t float_new(float f, pPmObj_t * r_pf);

/**
 * Implements the UNARY_NEGATIVE bcode.
 *
 * Creates a new float with a value that is the negative of the given float.
 *
 * @param   pobj Pointer to target object
 * @param   r_pint Return by ref, ptr to float
 * @return  Return status
 */
PmReturn_t float_negative(pPmObj_t pf, pPmObj_t * r_pf);

/**
 * Returns by reference a float that is x op y.
 *
 * @param px The float left-hand argument
 * @param py The float right-hand argument
 * @param r_pn The return value of x op y
 * @param op The operator (+,-,*,/ and power)
 * @return Return status
 */
PmReturn_t float_op(pPmObj_t px, pPmObj_t py, pPmObj_t * r_pn, int8_t op);

/**
 * Returns by reference a boolean that is x op y.
 *
 * @param px The float left-hand argument
 * @param py The float right-hand argument
 * @param r_pn The return value of x cmp y
 * @param cmp The comparison operator
 * @return Return status
 */
PmReturn_t float_compare(pPmObj_t px, pPmObj_t py, pPmObj_t * r_pobj,
			 PmCompare_t cmp);

#ifdef HAVE_PRINT
/**
 * Sends out a float object.
 * The number is preceded with a "-" when necessary.
 *
 * @param pObj Ptr to float object
 * @return Return status
 */
PmReturn_t float_print(pPmObj_t pf);

#endif				/* HAVE_PRINT */

#endif				/* HAVE_FLOAT */

#endif				/* __FLOAT_H__ */
