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

#ifndef __BYTEARRAY_H__
#define __BYTEARRAY_H__

/**
 * \file
 * \brief Bytearray Object Type
 *
 * Bytearray object type header.
 */

/**
 * Bytes container
 *
 * Holds actual byte payload
 */
typedef struct PmBytes_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Physical number of bytes in the C array (below) */
	uint16_t length;

    /** C array of bytes */
	uint8_t val[1];
} PmBytes_t, *pPmBytes_t;

/**
 * Bytearray obj
 *
 * Mutable ordered sequence of bytes.  Contains ptr to chunk of bytes.
 */
typedef struct PmBytearray_s {
    /** Object descriptor */
	PmObjDesc_t od;

    /** Bytearray length; logical number of bytes */
	uint16_t length;

    /** Ptr to bytes container (may hold more bytes than length) */
	pPmBytes_t val;
} PmBytearray_t, *pPmBytearray_t;

PmReturn_t bytearray_new(pPmObj_t pobj, pPmObj_t * r_pobj);
PmReturn_t bytearray_getItem(pPmObj_t pobj, int16_t index, pPmObj_t * r_pobj);
PmReturn_t bytearray_setItem(pPmObj_t pba, int16_t index, pPmObj_t pobj);
PmReturn_t bytearray_print(pPmObj_t pobj);

#endif				/* __BYTEARRAY_H__ */
