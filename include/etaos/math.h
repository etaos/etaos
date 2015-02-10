/*
 *  ETA/OS - Math functions
 *  Copyright (C) 2015  Michel Megens <dev@michelmegens.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MATH_H__
#define __MATH_H__

/**
 * @brief Rounds __x to the nearest integer.
 * @param __x Floating point number to round.
 */
extern double round(double __x);
/**
 * @brief Compute the cosine of \p __x.
 * @param __x Number to compute the cosine of.
 */
extern double cos(double __x);
/**
 * @brief Compute the sine of \p __x.
 * @param __x Number to compute the sine of.
 */
extern double sin(double __x);
/**
 * @brief Compute the tangent \p __x.
 * @param __x Number to compute the tangent of.
 */
extern double tan(double __x);
/**
 * @brief Compute the arc cosine.
 * @param __x Number to compute the arc cosine of.
 */
extern double acos(double __x);
/**
 * @brief Compute the arc sine.
 * @param __x Number to compute the arc sine of.
 */
extern double asin(double __x);
/**
 * @brief Compute the arc tangent
 * @param __x Number to compute the arc tangent of.
 */
extern double atan(double __x);
/**
 * @brief Compute the arc tangent of \p __y / \p __x.
 * @param __x Length of x.
 * @param __y Length of y.
 * @retval is in the range [-pi, +pi].
 */
extern double atan2(double __y, double __x);
/**
 * @brief Compute the square root of x.
 * @param __x Number to compute the sqrt of.
 */
extern double sqrt(double __x);
/**
 * @brief Round a number up.
 * @param __x Number to round up.
 */
extern double ceil(double __x);
/**
 * @brief Round a number down.
 * @param __x Number to round down.
 */
extern double floor(double __x);
/**
 * @brief Compute the natural logarithm of \p __x.
 * @param __x Number calculate the ln of.
 */
extern double log(double __x);
/**
 * @brief The log10() function return the logarithm of argument \p __x to base 10.
 * @param __x Number to calculate the log base 10 for.
 */
extern double log10(double __x);
/**
 * @brief Compute the exponential value of \p __x.
 * @param __x Number to compute the exponential value for.
 */
extern double exp(double __x);

#endif

