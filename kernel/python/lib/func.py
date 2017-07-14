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

## @addtogroup python-func
# @{

## @package func
#  @brief Provides PyMite's func module.


## Returns the names tuple of the function/module object
# @param f Function / module object.
# @return Names tuple.
#
def co_names(f):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pfunc;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    pfunc = NATIVE_GET_LOCAL(0);
    NATIVE_SET_TOS((pPmObj_t)((pPmFunc_t)pfunc)->f_co->co_names);

    return retval;
    """
    pass


## Returns the constants tuple of the function/module object
# @param f Function / module object.
# @return Constants tuple.
#
def co_consts(f):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pfunc;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    pfunc = NATIVE_GET_LOCAL(0);
    NATIVE_SET_TOS((pPmObj_t)((pPmFunc_t)pfunc)->f_co->co_consts);

    return retval;
    """
    pass

## @}

# :mode=c:
