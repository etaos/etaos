/*
 *  ETA/OS - Python VM
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

#undef __FILE_ID__
#define __FILE_ID__ 0x0E

/**
 * \file
 * \brief Module Object Type
 *
 * Module object type operations.
 */

#include <etaos/python.h>

PmReturn_t mod_new(pPmObj_t pco, pPmObj_t * pmod)
{
	PmReturn_t retval;
	uint8_t *pchunk;
	pPmObj_t pobj;
	uint8_t objid;

	/* If it's not a code obj, raise TypeError */
	if (OBJ_GET_TYPE(pco) != OBJ_TYPE_COB) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	/* Alloc and init func obj */
	retval = heap_getChunk(sizeof(PmFunc_t), &pchunk);
	PM_RETURN_IF_ERROR(retval);
	*pmod = (pPmObj_t) pchunk;
	OBJ_SET_TYPE(*pmod, OBJ_TYPE_MOD);
	((pPmFunc_t) * pmod)->f_co = (pPmCo_t) pco;
	((pPmFunc_t) * pmod)->f_attrs = C_NULL;
	((pPmFunc_t) * pmod)->f_globals = C_NULL;

#ifdef HAVE_DEFAULTARGS
	/* Clear the default args (only used by funcs) */
	((pPmFunc_t) * pmod)->f_defaultargs = C_NULL;
#endif				/* HAVE_DEFAULTARGS */

#ifdef HAVE_CLOSURES
	/* Clear field for closure tuple */
	((pPmFunc_t) * pmod)->f_closure = C_NULL;
#endif				/* HAVE_CLOSURES */

	/* Alloc and init attrs dict */
	heap_gcPushTempRoot(*pmod, &objid);
	retval = dict_new(&pobj);
	heap_gcPopTempRoot(objid);
	((pPmFunc_t) * pmod)->f_attrs = (pPmDict_t) pobj;

	/* A module's globals is the same as its attrs */
	((pPmFunc_t) * pmod)->f_globals = (pPmDict_t) pobj;

	return retval;
}

PmReturn_t mod_import(pPmObj_t pstr, pPmObj_t * pmod)
{
	PmMemSpace_t memspace;
	uint8_t const *imgaddr = C_NULL;
	pPmCo_t pco = C_NULL;
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t pobj;
	uint8_t objid;

	/* If it's not a string obj, raise SyntaxError */
	if (OBJ_GET_TYPE(pstr) != OBJ_TYPE_STR) {
		PM_RAISE(retval, PM_RET_EX_SYNTAX);
		return retval;
	}

	/* Try to find the image in the paths */
	retval = img_findInPaths(pstr, &memspace, &imgaddr);

	/* If img was not found, raise ImportError */
	if (retval == PM_RET_NO) {
		PM_RAISE(retval, PM_RET_EX_IMPRT);
		return retval;
	}

	/* Load img into code obj */
	retval = obj_loadFromImg(memspace, &imgaddr, &pobj);
	PM_RETURN_IF_ERROR(retval);
	pco = (pPmCo_t) pobj;

	heap_gcPushTempRoot(pobj, &objid);
	retval = mod_new((pPmObj_t) pco, pmod);
	heap_gcPopTempRoot(objid);

	return retval;
}
