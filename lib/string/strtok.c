/*
 *  ETA/OS - LibC strtok
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

/**
 * @addtogroup libc
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/string.h>

/**
 * @brief Find the first occurrance of a string.
 * @param s String to truncate. Please note that this string is modified.
 * @param delimeter Delimiter characters.
 * @return If a token is found, a pointer to the beginning of the token.
 * @retval NULL if no tokens are found.
 * @note This function is not thread safe.
 * @see strtok_r
 */
char *strtok(char *s, const char *delimeter)
{
	static char *tmp;

	return strtok_r(s, delimeter, &tmp);
}

/**
 * @brief Find the first occurrance of a string.
 * @param s String to truncate. Please note that this string is modified.
 * @param delim Delimiter characters.
 * @param last Used to store the current search context.
 * @return If a token is found, a pointer to the beginning of the token.
 * @retval NULL if no tokens are found.
 */
char *strtok_r(char *s, const char *delim, char **last)
{
	char *spanp;
	int c, sc;
	char *tok;

	if (s == NULL && (s = *last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *) delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	/* no non-delimiter characters */
	if (c == 0) {
		*last = NULL;
		return (NULL);
	}

	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *) delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */

	return NULL;
}

/** @} */

