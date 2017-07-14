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

#ifndef __MODULE_H__
#define __MODULE_H__

/**
 * \file
 * \brief Module Object Type
 *
 * Module object type header.
 */

/**
 * Creates a Module Obj for the given Code Obj.
 *
 * Use a func struct to represent the Module obj because
 * the module's construction code must execute later,
 * but set the type to OBJ_TYPE_MOD so that it is
 * not otherwise callable.
 *
 * @param   pco Ptr to code obj
 * @param   pmod Return by reference; ptr to new module obj
 * @return  Return status
 */
PmReturn_t mod_new(pPmObj_t pco, pPmObj_t * pmod);

/**
 * Imports a module of the given name.
 * Searches for an image with a matching name.
 * A code obj is created for the code image.
 * A module obj is created for the code obj.
 *
 * @param   pstr String obj containing name of code obj to load.
 * @param   pmod Return by reference; ptr to imported module
 * @return  Return status
 */
PmReturn_t mod_import(pPmObj_t pstr, pPmObj_t * pmod);

#endif				/* __MODULE_H__ */
