/*
 *  ETA/OS - LibC ltoa
 *  Copyright (C) 2016  Michel Megens <dev@bietje.net>
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
#include <etaos/stdlib.h>
#include <etaos/types.h>

#include "stdlib_helper.h"

char *ltoa(long num, char *str, int base)
{
	int remainder;
	char *rv;
	bool neg = false;

	rv = str;

	if(!num) {
		*str++ = '0';
		*str = '\0';
		return rv;
	}

	/* Only handle negative numbers in base 10 */
	if(num < 0L && base == 10) {
		neg = true;
		num = -num;
	}

	while(num) {
		remainder = num % base;
		if(remainder > 9)
			*str++ = (remainder - 10) + 'a';
		else
			*str++ = remainder + '0';
		num /= base;
	}

	if(neg)
		*str++ = '-';

	*str++ = '\0';
	strrev(rv);

	return rv;
}

