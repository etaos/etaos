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

#ifndef __FUNC_H__
#define __FUNC_H__

/**
 * \file
 * \brief Function Object Type
 *
 * Function object type header.
 */

/**
 * Function obj
 *
 * A function is like an instance of a code obj.
 * Contains ptr to its code obj and has its own attributes dict.
 *
 * The first (__main__) module that is executed has a function obj
 * created for it to execute the bytecode which builds the module.
 */
typedef struct PmFunc_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Ptr to code obj */
	pPmCo_t f_co;

    /** Ptr to attribute dict */
	pPmDict_t f_attrs;

    /** Ptr to globals dict */
	pPmDict_t f_globals;

#ifdef HAVE_DEFAULTARGS
    /** Ptr to tuple holding default args */
	pPmTuple_t f_defaultargs;
#endif				/* HAVE_DEFAULTARGS */

#ifdef HAVE_CLOSURES
    /** Ptr to tuple of cell values */
	pPmTuple_t f_closure;
#endif				/* HAVE_CLOSURES */

} PmFunc_t, *pPmFunc_t;

/**
 * Creates a Function Obj for the given Code Obj.
 * Allocate space for a Func obj and fill the fields.
 *
 * @param   pco ptr to code obj
 * @param   pglobals ptr to globals dict (from containing func/module)
 * @param   r_pfunc Return by reference; pointer to new function
 * @return  Return status
 */
PmReturn_t func_new(pPmObj_t pco, pPmObj_t pglobals, pPmObj_t * r_pfunc);

#endif				/* __FUNC_H__ */
