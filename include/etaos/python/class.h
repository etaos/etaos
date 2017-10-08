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

#ifndef __CLASS_H__
#define __CLASS_H__

/**
 * \file
 *  \brief Class header.
 */

/**
 * Class struct
 *
 * This C struct is used for PyMite class objects
 * Note: Exceptions are objects.
 */
typedef struct PmClass_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Attributes dict */
	pPmDict_t cl_attrs;

    /** Bases tuple */
	pPmTuple_t cl_bases;
} PmClass_t, *pPmClass_t;

/** Class instance struct */
typedef struct PmInstance_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Class of this instance */
	pPmClass_t cli_class;

    /** Attributes dict */
	pPmDict_t cli_attrs;
} PmInstance_t, *pPmInstance_t;

/** Method struct */
typedef struct PmMethod_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Class instance of this method */
	pPmInstance_t m_instance;

    /** Func of this method */
	pPmFunc_t m_func;

    /** Attributes dict */
	pPmDict_t m_attrs;
} PmMethod_t, *pPmMethod_t;

/**
 * Creates a new Class object from the methods dict, bases tuple,
 * and name string.
 *
 * @param   pmeths ptr to methods dict.
 * @param   pbases ptr to bases tuple.
 * @param   pname ptr to name string.
 * @param   r_pclass Return by ref, ptr to new class
 * @return  Return status
 */
PmReturn_t class_new(pPmObj_t pmeths, pPmObj_t pbases, pPmObj_t pname,
		     pPmObj_t * r_pclass);

/**
 * Returns an instance of the given class
 *
 * @param pclass Pointer to class object
 * @param r_pobj Return by ref, instance object
 * @return  Return status
 */
PmReturn_t class_instantiate(pPmObj_t pclass, pPmObj_t * r_pobj);

#ifdef HAVE_AUTOBOX
/**
 * Autoboxes an object in place
 *
 * @param pclass Pointer to object
 * @return  Return status
 */
PmReturn_t class_autobox(pPmObj_t * pobj);
#endif

/**
 * Returns a method based on the given inputs
 *
 * @param   pinstance ptr to instance
 * @param   pfunc ptr to func
 * @param   r_pmeth Return by ref, ptr to new method
 * @return  Return status
 */
PmReturn_t class_method(pPmObj_t pinstance, pPmObj_t pfunc, pPmObj_t * r_pmeth);

/**
 * Returns the first attribute named __init__ in the class' inheritance tree
 *
 * @param   pobj ptr to class or instance to search
 * @param   pname ptr to name of attr to find
 * @param   r_pobj Return by ref, ptr to attr if found, or undetermined
 * @return  Return status
 */
PmReturn_t class_getAttr(pPmObj_t pobj, pPmObj_t pname, pPmObj_t * r_pobj);

/**
 * Returns a C boolean if the base class is found in the inheritance tree
 * of the test class.  NOTE: This function is recursive.
 *
 * @param   ptest_class ptr to class whose inheritance tree is searched
 * @param   pbase_class ptr to class to look for
 * @return  Returns C_TRUE if pbase_class is found in the inheritance tree;
 *          C_FALSE otherwise.
 */
uint8_t class_isSubclass(pPmObj_t ptest_class, pPmObj_t pbase_class);

extern PmReturn_t class_get_attrib(pPmObj_t obj, const char *name, pPmObj_t *r_obj);
extern PmReturn_t class_set_attr(pPmObj_t obj, pPmObj_t attr, pPmObj_t value);
extern PmReturn_t class_set_attr_cstr(pPmObj_t obj, const char *c_attr, pPmObj_t value);

#endif				/* __CLASS_H__ */
