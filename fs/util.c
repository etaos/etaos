/*
 *  ETA/OS - File system utility functions
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/string.h>
#include <etaos/types.h>
#include <etaos/mem.h>

#include <etaos/fs/util.h>

static char **path_split(char *path, const char _delim)
{
	char *tmp = path, *token;
	size_t count = 0UL, idx;
	char *last = NULL;
	char **result = NULL;
	char delim[] = { _delim, 0 };
	char *strtok_last = NULL;

	while(*tmp) {
		if(*tmp == _delim) {
			count ++;
			last = tmp;
		}

		tmp++;
	}

	/* Add space for trailing tokens */
	count += last < (path + strlen(path) - 1);

	/* Space for NULL terminator */
	count ++;

	result = kzalloc(sizeof(char*) * count);

	if(!result)
		return NULL;

	idx = 0UL;
	token = strtok_r(path, delim, &strtok_last);

	while(token) {
		*(result + idx++) = strdup(token);
		token = strtok_r(0, delim, &strtok_last);
	}

	*(result + idx) = NULL;
	return result;
}

char **fs_split_path(const char *__path__)
{
	char *path;
	char **result = NULL;

	path = strdup(__path__);
	result = path_split(path, '/');
	kfree(path);

	return result;
}

