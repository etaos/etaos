/*
 *  ETA/OS - LibC random number generation
 *  Copyright (C) 2015   Michel Megens <dev@michelmegens.net>
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
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>

static uint32_t do_random(uint32_t *seed, uint32_t max)
{
	long high, low, x;

	x = *seed;
	if(!x)
		x = 123459876L;

	high = x / 127773L;
	low = x % 127773L;
	x = 16807L * low - 2836L * high;
	if(x < 0)
		x += 0x7FFFFFFFL;

	return ((*seed = x) % (max + 1));
}

static uint32_t __seed = 1;

/**
 * @brief Set the seed value for the RNG.
 * @param seed New seed value.
 */
void srand(uint32_t seed)
{
	__seed = seed;
}

/**
 * @brief Generate a new random number, with a given maximum.
 * @param max Maximum number to generate.
 *
 * The output of random_m(x) will have the range of [0, x].
 */
uint32_t random_m(uint32_t max)
{
	return do_random(&__seed, max);
}

#define RAND_MAX 0x7FFF

/**
 * @brief Generate a random number.
 * @retval A (pseudo) random number.
 */
uint32_t random(void)
{
	return do_random(&__seed, RAND_MAX);
}

/**
 * @brief Generate a random number, while giving a seed and maximum.
 * @param sptr Pointer to the seed to use.
 * @param max Maximum number to generate.
 */
uint32_t random_r(uint32_t *sptr, uint32_t max)
{
	if(!max)
		max = RAND_MAX;

	return do_random(sptr, max);
}

/** @} */
