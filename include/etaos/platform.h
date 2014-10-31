/*
 *  ETA/OS - Platform driver
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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

/**
 * @file etaos/platform.h
 */

/**
 * @addtogroup platform
 */
/* @{ */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

CDECL

/**
 * @brief Write a value to a platform pin.
 * @param pin pin to write to.
 * @param val value to write.
 * @return Error code.
 * @retval -EOK success.
 * @retval -EINVAL on error.
 */
extern int pgpio_write_pin(int pin, int val);

/**
 * @brief Read the value of a platform pin.
 * @param pin Pin to read from.
 * @return The value read from the pin.
 * @retval -EINVAL on error.
 */
extern int pgpio_read_pin(int pin);

/**
 * @brief Set the direction of a platform pin to output.
 * @param pin Pin to configure as output.
 * @param val Value to write to the output pin.
 * @return Error code.
 * @retval -EOK success.
 * @retval -EINVAL on error.
 */
extern int pgpio_direction_output(int pin, int val);

/**
 * @brief Set a platform pin as input pin.
 * @param pin Pin to configure as input.
 * @return Error code.
 * @retval -EOK success.
 * @retval -EINVAL on error.
 */
extern int pgpio_direction_input(int pin);

/**
 * @brief Release a requested platform pin.
 * @param pin Pin to release.
 * @return Error code.
 * @retval 1 if the pin was never requested.
 * @retval -EOK on success.
 */
extern int pgpio_pin_release(int pin);

/**
 * @brief Request a platform GPIO pin.
 * @param pin Pin to request.
 * @return Error code.
 * @retval -EINVAL pin could not be requested.
 * @retval -EOK on success.
 */
extern int pgpio_pin_request(int pin);

CDECL_END

#endif

/* @} */
