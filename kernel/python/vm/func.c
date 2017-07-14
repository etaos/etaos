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
#define __FILE_ID__ 0x04

/**
 * \file
 * \brief Function Object Type
 *
 * Function object type operations.
 */

#include <etaos/python.h>

PmReturn_t func_new(pPmObj_t pco, pPmObj_t pglobals, pPmObj_t * r_pfunc)
{
	PmReturn_t retval = PM_RET_OK;
	pPmFunc_t pfunc = C_NULL;
	uint8_t *pchunk;
	pPmObj_t pobj;
	uint8_t objid;

	C_ASSERT(OBJ_GET_TYPE(pco) != OBJ_TYPE_COB
		 || OBJ_GET_TYPE(pco) != OBJ_TYPE_NOB);
	C_ASSERT(OBJ_GET_TYPE(pglobals) == OBJ_TYPE_DIC);

	/* Allocate a func obj */
	retval = heap_getChunk(sizeof(PmFunc_t), &pchunk);
	PM_RETURN_IF_ERROR(retval);
	pfunc = (pPmFunc_t) pchunk;

	/* Init func */
	OBJ_SET_TYPE(pfunc, OBJ_TYPE_FXN);
	pfunc->f_co = (pPmCo_t) pco;
	pfunc->f_globals = C_NULL;
	pfunc->f_attrs = C_NULL;

#ifdef HAVE_DEFAULTARGS
	/* Clear default args (will be set later, if at all) */
	pfunc->f_defaultargs = C_NULL;
#endif				/* HAVE_DEFAULTARGS */

#ifdef HAVE_CLOSURES
	/* Clear field for closure tuple */
	pfunc->f_closure = C_NULL;
#endif				/* HAVE_CLOSURES */

	/* Create attrs dict for regular func (not native) */
	if (OBJ_GET_TYPE(pco) == OBJ_TYPE_COB) {
		heap_gcPushTempRoot((pPmObj_t) pfunc, &objid);
		retval = dict_new(&pobj);
		heap_gcPopTempRoot(objid);
		PM_RETURN_IF_ERROR(retval);
		pfunc->f_attrs = (pPmDict_t) pobj;

		/* Store the given globals dict */
		pfunc->f_globals = (pPmDict_t) pglobals;
	}

	*r_pfunc = (pPmObj_t) pfunc;
	return PM_RET_OK;
}
