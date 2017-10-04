/*
 *  ETA/OS - String Compare
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

/**
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/assert.h>
#include <etaos/stdio.h>

const char **strsplit(const char *s, const char delim)
{
	char **result;
	size_t count;
	char *tmp, *dup;
	char *last = NULL;
	const char sdelim[2] = {delim, '\0'};
	size_t idx;
	char *token, *context;

	dup = tmp = strdup(s);
	count = 0;

	while(*tmp) {
		if(*tmp == delim) {
			count += 1;
			last = tmp;
		}

		tmp += 1;
	}

	/*
	 * Add space for trailing tokens and null terminators
	 */
	count += last < (s + strlen(s) - 1);
	count ++;
	result = kzalloc(sizeof(s) * count);

	if(unlikely(!result))
		return NULL;

	idx = 0;
	token = strtok_r(dup, sdelim, &context);
	while(token) {
		ASSERT(idx < count);
		*(result + idx) = strdup(token);
		idx += 1;
		token = strtok_r(NULL, sdelim, &context);
	}

	*(result + idx) = NULL;
	kfree(dup);
	return (const char**)result;
}

/** @} */
