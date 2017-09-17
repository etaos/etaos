/*
 *  ETA/OS - Math functions
 *  Copyright (C) 2015  Michel Megens <dev@bietje.net>
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

#ifndef __MATH_H__
#define __MATH_H__

/**
 * @addtogroup math
 * @{
 */

/**
 * @brief Rounds __x to the nearest integer.
 * @param __x Floating point number to round.
 * @return The rounded number.
 */
extern double round(double __x);

/**
 * @brief Calculate the hypotenuse of a right angled triangle.
 * @param x Floating point value.
 * @param y Floating point value.
 * @return The square root of the sum of x and y squared.
 */
extern double hypot(double x, double y);
/**
 * @brief Compute the cosine of \p __x.
 * @param __x Number to compute the cosine of.
 * @return The cosine of __x.
 */
extern double cos(double __x);
/**
 * @brief Compute the sine of \p __x.
 * @param __x Number to compute the sine of.
 * @return The sine of __x.
 */
extern double sin(double __x);
/**
 * @brief Compute the tangent \p __x.
 * @param __x Number to compute the tangent of.
 * @return The tangent of __x.
 */
extern double tan(double __x);
/**
 * @brief Compute the arc cosine.
 * @param __x Number to compute the arc cosine of.
 * @return The inverse cosine of __x.
 */
extern double acos(double __x);
/**
 * @brief Compute the arc sine.
 * @param __x Number to compute the arc sine of.
 * @return The inverse sine of __x.
 */
extern double asin(double __x);
/**
 * @brief Compute the arc tangent
 * @param __x Number to compute the arc tangent of.
 * @return The inverse tangent of __x.
 */
extern double atan(double __x);
/**
 * @brief Compute the arc tangent of \p __y / \p __x.
 * @param __x Length of x.
 * @param __y Length of y.
 * @return The inverse tan2 of __y and __x.
 * @retval is in the range [-pi, +pi].
 */
extern double atan2(double __y, double __x);
/**
 * @brief Compute the square root of x.
 * @param __x Number to compute the sqrt of.
 * @return The square root of __x.
 */
extern double sqrt(double __x);
/**
 * @brief Round a number up.
 * @param __x Number to round up.
 * @return The ceiling of __x.
 */
extern double ceil(double __x);
/**
 * @brief Round a number down.
 * @param __x Number to round down.
 * @return The floor of __x.
 */
extern double floor(double __x);
/**
 * @brief Compute the natural logarithm of \p __x.
 * @param __x Number calculate the ln of.
 * @return The natural log of __x.
 */
extern double log(double __x);
/**
 * @brief The log10() function return the logarithm of argument \p __x to base 10.
 * @param __x Number to calculate the log base 10 for.
 * @return The log10 of __x.
 */
extern double log10(double __x);
/**
 * @brief Compute the exponential value of \p __x.
 * @param __x Number to compute the exponential value for.
 * @return The exponential value of __x.
 */
extern double exp(double __x);

/**
 * @brief Compute the remainder of the divion of x by y.
 * @param x Number to divide.
 * @param y Number to divide by.
 * @return The return value is  x - n*y, where n is the quotient of x/y,
 *         rounded towards the nearest whole integer.
 */
extern double fmod(double x, double y);

/**
 * @brief Compute the y-th power of x.
 * @param x Base number to compute the power of.
 * @param y Power to raise \p x to.
 * @return The result of x^y.
 */
extern double pow(double x, double y);

#define fmodf(__x__, __y__) fmod(__x__, __y__)
#define powf(__x__, __y__) pow(__x__, __y__)

#define to_ns(__s) (__s * 1E9) //!< Convert seconds to nanoseconds
#define to_us(__s) (__s * 1E6) //!< Convert seconds to microseconds
#define to_ms(__s) (__s * 1E3) //!< Convert seconds to miliseconds

/** @} */

#endif
