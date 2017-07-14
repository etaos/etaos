#
#   ETA/OS - CPU class
#   Copyright (C) 2017  Dean Hall
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

## @addtogroup python-list
# @{

## @package list
#  @brief Provides PyMite's list module.
#
# Notes:
# - index(l, o) does not offer start and stop arguments.


__name__ = "list"

class _Autobox:
    def append(self, o):
        return append(self.obj, o)

    def count(self, v):
        return count(self.obj, v)

    def extend(self, s):
        return extend(self.obj, s)

    def index(self, o):
        return index(self.obj, o)

    def insert(self, i, o):
        return insert(self.obj, i, o)

    def pop(self, i=None):
        if i==None:
            return pop(self.obj)
        else:
            return pop(self.obj, i)

    def remove(self, v):
        return remove(self.obj, v)

## Append to list.
# @param l List object
# @param o Object to append.
# @return None.
def append(l, o):
    """__NATIVE__
    pPmObj_t pl;
    PmReturn_t retval = PM_RET_OK;

    /* Raise TypeError if it's not a list or wrong number of args, */
    pl = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pl) != OBJ_TYPE_LST) || (NATIVE_GET_NUM_ARGS() != 2))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Append the object to the list */
    retval = list_append(pl, NATIVE_GET_LOCAL(1));

    NATIVE_SET_TOS(PM_NONE);

    return retval;
    """
    pass

## Count occurances of \p v in \p l.
# @param l List object
# @param v Object to count.
# @return Number of times \p v was found in \p l.
def count(l, v):
    c = 0
    for o in l:
        if o == v:
            c = c + 1
    return c

## Add the elements of \p s to \p l.
# @param l List object.
# @param s List object.
# @return None.
def extend(l, s):
    for i in s:
        append(l, i)

## Get the index of the first occurance of \p o in \p l.
# @param l List object
# @param o Object to search for.
# @return Index of the first occurance of \p o in \p l.
def index(l, o):
    """__NATIVE__
    pPmObj_t pl;
    pPmObj_t po;
    pPmObj_t pi;
    PmReturn_t retval = PM_RET_OK;
    uint16_t i;

    /* Raise TypeError if it's not a list or wrong number of args, */
    pl = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pl) != OBJ_TYPE_LST) || (NATIVE_GET_NUM_ARGS() != 2))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get the index of the object in the list */
    po = NATIVE_GET_LOCAL(1);
    retval = list_index(pl, po, &i);

    if (retval == PM_RET_EX_VAL)
    {
        PM_RAISE(retval, PM_RET_EX_VAL);
        return retval;
    }

    int_new((int32_t)i, &pi);
    NATIVE_SET_TOS(pi);

    return retval;
    """
    pass

## Insert \p o at \p i in \p l.
# @param l List object.
# @param i Index to insert at.
# @param o Object to insert.
# @return None.
def insert(l, i, o):
    """__NATIVE__
    pPmObj_t pl;
    pPmObj_t pi;
    pPmObj_t po;
    PmReturn_t retval = PM_RET_OK;
    uint16_t i;

    /*
     * Raise TypeError if wrong number of args, first arg is not a list, or
     * second arg is not an int
     */
    pl = NATIVE_GET_LOCAL(0);
    pi = NATIVE_GET_LOCAL(1);
    po = NATIVE_GET_LOCAL(2);
    if ((NATIVE_GET_NUM_ARGS() != 3)
        || (OBJ_GET_TYPE(pl) != OBJ_TYPE_LST)
        || (OBJ_GET_TYPE(pi) != OBJ_TYPE_INT) )
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Insert the object before the given index */
    i = (uint16_t)((pPmInt_t)pi)->val;
    retval = list_insert(pl, i, po);

    if (retval != PM_RET_OK)
    {
        PM_RAISE(retval, PM_RET_EX_SYS);
    }

    NATIVE_SET_TOS(PM_NONE);

    return retval;
    """
    pass

## Pop item \p i of list \p l.
# @param l List object.
# @param i Index to pop.
# @return The item stored at \p i.
def pop(l, i):
    """__NATIVE__
    pPmObj_t pl;
    pPmObj_t pi;
    pPmObj_t po;
    PmReturn_t retval = PM_RET_OK;
    int16_t i;

    /*
     * Raise TypeError if first arg is not a list o second arg is not an int
     * or there are the wrong number of arguments
     */
    pl = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(pl) != OBJ_TYPE_LST)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    pi = NATIVE_GET_LOCAL(1);
    if (NATIVE_GET_NUM_ARGS() == 2)
    {
        if (OBJ_GET_TYPE(pi) != OBJ_TYPE_INT)
        {
            PM_RAISE(retval, PM_RET_EX_TYPE);
            return retval;
        }
        i = (uint16_t)((pPmInt_t)pi)->val;
    }
    else
    {
        i = -1;
    }
    if ((NATIVE_GET_NUM_ARGS() < 1) || (NATIVE_GET_NUM_ARGS() > 2))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get the object at the given index */
    retval = list_getItem(pl, i, &po);
    PM_RETURN_IF_ERROR(retval);

    /* Return the object to the caller */
    NATIVE_SET_TOS(po);

    /* Remove the object from the given index */
    retval = list_delItem(pl, i);
    PM_RETURN_IF_ERROR(retval);

    return retval;
    """
    pass

## Remove the first match of \p v in \p l.
# @param l List object.
# @param v Object to search for.
# @return None.
def remove(l, v):
    """__NATIVE__
    pPmObj_t pl;
    pPmObj_t pv;
    PmReturn_t retval = PM_RET_OK;

    /* Raise TypeError if it's not a list or wrong number of args, */
    pl = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pl) != OBJ_TYPE_LST) || (NATIVE_GET_NUM_ARGS() != 2))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Remove the value from the list */
    pv = NATIVE_GET_LOCAL(1);
    retval = list_remove(pl, pv);
    if (retval != PM_RET_OK)
    {
        PM_RAISE(retval, retval);
    }

    NATIVE_SET_TOS(PM_NONE);

    return retval;
    """
    pass

## @}

# :mode=c:
