#
#   ETA/OS - SRAM library
#   Copyright (C) 2017  Michel Megens <dev@bietje.net>
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

## @addtogroup python-math
# @{

## @package math
#  @brief Provides PyMite's maths module
#
#  USAGE
#  -----
#
#  import sys
#

"""__NATIVE__
#include <etaos/stdlib.h>
#include <etaos/types.h>
#include <etaos/math.h>

static float math_require_float(pPmObj_t number)
{
    PmReturn_t retval;

    if (OBJ_GET_TYPE(number) == OBJ_TYPE_INT) {
        return (float)((pPmInt_t)number)->val;
    } else if(OBJ_GET_TYPE(number) == OBJ_TYPE_FLT) {
        return ((pPmFloat_t)number)->val;
    } else {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        (void)retval;
        return -1.0f;
    }
}

static float float_get_args1(void)
{
    pPmObj_t obj;
    PmReturn_t retval;

    if (NATIVE_GET_NUM_ARGS() != 1) {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return -1.0f;
    }

    obj = NATIVE_GET_LOCAL(0);

    if (OBJ_GET_TYPE(obj) != OBJ_TYPE_INT && OBJ_GET_TYPE(obj) != OBJ_TYPE_FLT) {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return -1.0f;
    }

    (void)retval;
    return math_require_float(obj);
}

static float float_get_args(int num, int n)
{
    pPmObj_t obj;
    PmReturn_t retval;

    if (NATIVE_GET_NUM_ARGS() != num) {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return -1.0f;
    }

    obj = NATIVE_GET_LOCAL(n);

    if (OBJ_GET_TYPE(obj) != OBJ_TYPE_INT && OBJ_GET_TYPE(obj) != OBJ_TYPE_FLT) {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return -1.0f;
    }

    (void)retval;
    return math_require_float(obj);
}
"""

__name__ = "math"

pi = 3.141592
e = 2.718281

## Round a number.
# @param n Integer or floating point number.
# @return Rounded value of \p n.
def round(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = round(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the cosine of \p n.
# @param n Integer or floating point number.
# @return Cosine of \p n.
def cos(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = cos(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the sine of \p n.
# @param n Integer or floating point number.
# @return Sine of \p n.
def sin(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = sin(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the tangent of \p n.
# @param n Integer or floating point number.
# @return Tangent of \p n.
def tan(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = tan(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the arc tangent of \p n.
# @param n Integer or floating point number.
# @return Arc tangent of \p n.
def atan(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = atan(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the arc tangent of m/n.
# @param n Integer or floating point number.
# @param m Integer or floating point number.
# @return The angle between the x-plane and the given coordinates \p n, \p m.
def atan2(n, m):
    """__NATIVE__
    float n, m, rv;
    pPmObj_t robj;
    PmReturn_t retval;

    n = float_get_args(2,0);
    m = float_get_args(2,1);
    rv = atan2(n,m);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the square root of n.
# @param n Integer or floating point number.
# @return The square root of \p n.
def sqrt(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = sqrt(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the hypotenuse of \p x and \p y.
# @param x Integer or floating point number.
# @param y Integer or floating point number.
# @return The hypotenuse.
def hypot(x, y):
    m = x*x
    n = y*y
    return sqrt(m+n)


## Round a number up.
# @param n Integer or floating point number.
# @return Rounded value of \p n.
def ceil(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = ceil(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Round a number down.
# @param n Integer or floating point number.
# @return Rounded value of \p n.
def floor(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = floor(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the exponent of \p n.
# @param n Integer or floating point number.
# @return The base \p e exponential function of \p n.
def exp(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = exp(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the base 10 log.
# @param n Integer or floating point number.
# @return The base 10 log of \p n.
def log10(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = log10(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the natural logarithm of \p n.
# @param n Integer or floating point number.
# @return The natural logarithm of \p n.
def log(n):
    """__NATIVE__
    float rv;
    pPmObj_t robj;
    PmReturn_t retval;

    rv = float_get_args1();
    rv = log(rv);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate the modulo of \p n and \p m.
# @param n Integer or floating point number.
# @param m Integer or floating point number.
# @return The modulo of \p n and \p m.
def mod(n, m):
    """__NATIVE__
    float rv, n, m;
    pPmObj_t robj;
    PmReturn_t retval;

    n = float_get_args(2, 0);
    m = float_get_args(2, 1);
    rv = fmod(n, m);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## Calculate \p n to the power of \p m.
# @param n Integer or floating point number.
# @param m Integer or floating point number.
# @return The result of \p n to the mth power.
def pow(n, m):
    """__NATIVE__
    float rv, n, m;
    pPmObj_t robj;
    PmReturn_t retval;

    n = float_get_args(2, 0);
    m = float_get_args(2, 1);
    rv = pow(n, m);
    retval = float_new(rv, &robj);
    NATIVE_SET_TOS(robj);

    return retval;
    """
    pass

## @}
