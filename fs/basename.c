/*
 *  ETA/OS - Directory entry
 *  Copyright (C) 2012, 2014, 2015   Michel Megens
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
 * @addtogroup fops
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/stdlib.h>
#include <etaos/vfs.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/list.h>

/**
 * @brief Return the base name of a path.
 * @param path Path to get the base name of.
 * @note The return value is a newly allocated string.
 * @note The base name of the path \p /var/www/html would be \p html.
 * @return The base name \p path.
 */
char *basename(const char *path)
{
	int len, idx;
	char *basename = NULL;

	if(!path || *path == '\0')
		return NULL;

	len = strlen(path) - 1;
	
	for(idx = len; idx >= 0; idx--) {
		if(path[idx] == '/') {
			len = len - idx + 1;
			basename = kzalloc(len);
			strncpy(basename, &path[idx + 1], len - 1);
			basename[len - 1] = '\0';
			break;
		} else if(idx == 0) {
			basename = kzalloc(len + 2);
			strcpy(basename, path);
		}
	}

	return basename;
}

/**
 * @brief Return the base path of a path.
 * @param path Path to get the base path of.
 * @return The base path of \p path.
 * @note The return value is an allocated string.
 * @note The base path of \p /var/www/html would be \p /var/www.
 */
char *basepath(const char *path)
{
	int len, idx;
	char *basepath = NULL;

	if(!path)
		return NULL;

	/* Length as an index to path */
	len = strlen(path) - 1;
	
	for(idx = len; idx >= 0; idx--) {
		if(path[idx] == '/' && idx != 0) {
			len = idx + 1;
			basepath = kzalloc(len);
			strncpy(basepath, path, len - 1);
			basepath[len - 1] = '\0';
			break;
		} else if(idx == 0) {
			basepath = kzalloc(2);
			basepath[0] = '/';
		}
	}

	return basepath;
}

/** @} */

