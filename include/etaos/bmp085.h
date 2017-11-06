/*
 *  ETA/OS - BMP085 header
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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

#ifndef __BMP085_SENSOR_H__
#define __BMP085_SENSOR_H__

/**
 * @addtogroup bmp085
 * @{
 */

/**
 * @brief BMP085 `ioctl()` options.
 */
typedef enum {
	BMP_MEASURE_TEMPERATURE,
	BMP_MEASURE_PRESSURE,
} bmp085_ioctl_t;

/** @} */
#endif
