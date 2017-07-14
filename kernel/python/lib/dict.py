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

## @addtogroup python-dict
# @{

## @package dict
#  @brief Provides PyMite's dict module.


__name__ = "dict"

class _Autobox:
    def clear(self):
        return clear(self.obj)

    def keys(self):
        return keys(self.obj)

    def has_key(self, k):
        return has_key(self.obj, k)

    def values(self):
        return values(self.obj)

## Clear a dictionary.
# @param d Dictionary to clear.
# @return None.
def clear(d):
    """__NATIVE__
    pPmObj_t pd;
    PmReturn_t retval = PM_RET_OK;

    /* Raise TypeError if it's not a dict or wrong number of args, */
    pd = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pd) != OBJ_TYPE_DIC) || (NATIVE_GET_NUM_ARGS() != 1))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Clear the contents of the dict */
    retval = dict_clear(pd);
    PM_RETURN_IF_ERROR(retval);

    NATIVE_SET_TOS(PM_NONE);

    return retval;
    """
    pass

## Return a list of keys in \p d.
# @param d Dictionary
# @return Keys of \p d.
def keys(d):
    """__NATIVE__
    pPmObj_t pd;
    pPmObj_t pl;
    pPmObj_t pk;
    pSeglist_t psl;
    uint16_t i;
    PmReturn_t retval = PM_RET_OK;
    uint8_t objid;

    /* Raise TypeError if it's not a dict or wrong number of args, */
    pd = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pd) != OBJ_TYPE_DIC) || (NATIVE_GET_NUM_ARGS() != 1))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create empty list */
    retval = list_new(&pl);
    PM_RETURN_IF_ERROR(retval);

    /* Iterate through the keys seglist */
    psl = ((pPmDict_t)pd)->d_keys;
    for (i = 0; i < ((pPmDict_t)pd)->length; i++)
    {
        /* Get the key and append it to the list */
        retval = seglist_getItem(psl, i, &pk);
        PM_RETURN_IF_ERROR(retval);
        heap_gcPushTempRoot(pl, &objid);
        retval = list_append(pl, pk);
        heap_gcPopTempRoot(objid);
        PM_RETURN_IF_ERROR(retval);
    }

    /* Return the list of keys to the caller */
    NATIVE_SET_TOS(pl);

    return retval;
    """
    pass

## Check if \p k is in \p d.
# @param d Dictionary to check.
# @param k Key to check for.
# @return Boolean value.
def has_key(d, k):
    return k in d

## Return the value list of \p d.
# @param d Dictionary to get the values for.
# @return The values found in \p d.
def values(d):
    """__NATIVE__
    pPmObj_t pd;
    pPmObj_t pl;
    pPmObj_t pv;
    pSeglist_t psl;
    uint16_t i;
    PmReturn_t retval = PM_RET_OK;
    uint8_t objid;

    /* Raise TypeError if it's not a dict or wrong number of args, */
    pd = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pd) != OBJ_TYPE_DIC) || (NATIVE_GET_NUM_ARGS() != 1))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create empty list */
    retval = list_new(&pl);
    PM_RETURN_IF_ERROR(retval);

    /* Iterate through the values seglist */
    psl = ((pPmDict_t)pd)->d_vals;
    for (i = 0; i < ((pPmDict_t)pd)->length; i++)
    {
        /* Get the value and append it to the list */
        retval = seglist_getItem(psl, i, &pv);
        PM_RETURN_IF_ERROR(retval);
        heap_gcPushTempRoot(pl, &objid);
        retval = list_append(pl, pv);
        heap_gcPopTempRoot(objid);
        PM_RETURN_IF_ERROR(retval);
    }

    /* Return the list of values to the caller */
    NATIVE_SET_TOS(pl);

    return retval;
    """
    pass

## Update \p d1 with the contents of \p d2.
# @param d1 Dictionary
# @param d2 Dictionary
# @return None
def update(d1, d2):
    """__NATIVE__
    pPmObj_t pd1;
    pPmObj_t pd2;
    PmReturn_t retval;

    /* Raise TypeError if wrong number of args, */
    if (NATIVE_GET_NUM_ARGS() != 2)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    pd1 = NATIVE_GET_LOCAL(0);
    pd2 = NATIVE_GET_LOCAL(1);
    retval = dict_update(pd1, pd2, C_FALSE);

    NATIVE_SET_TOS(PM_NONE);
    return retval;
    """
    pass

## @}

# :mode=c:
