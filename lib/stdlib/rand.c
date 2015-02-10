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
void srand(uint32_t seed)
{
	__seed = seed;
}

uint32_t random_m(uint32_t max)
{
	return do_random(&__seed, max);
}

#define RAND_MAX 0x7FFF

uint32_t random(void)
{
	return do_random(&__seed, RAND_MAX);
}

uint32_t random_r(uint32_t *sptr, uint32_t max)
{
	if(!max)
		max = RAND_MAX;

	return do_random(sptr, max);
}

