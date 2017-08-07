/*
 *  ETA/OS - String Compare
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>, Ethernut
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

/**
 * @addtogroup libc
 * @{
 */

static const uint8_t charmap[] = {
    '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
    '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
    '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
    '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
    '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
    '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
    '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
    '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
    '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
    '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
    '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
    '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
    '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
    '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
    '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
    '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
    '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
    '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
    '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
    '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
    '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
    '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
    '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
    '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
    '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
    '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
    '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
    '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
    '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
    '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
    '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
    '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

/**
 * @brief Compare two strings, ignoring case.
 * @param s1 String 1.
 * @param s2 String 2.
 * @return An integer greater than, equal to or less than 0 according to whether
 *         \p s1 is greater than, equal to or less than \p s2.
 */
int stricmp(const char *s1, const char *s2)
{
	const uint8_t *map = charmap,
	              *us1 = (uint8_t*)s1,
		      *us2 = (uint8_t*)s2;

	while(map[*us1] == map[*us2++]) {
		if(*us1 == '\0')
			return 0;
	}

	return map[*us1] - map[*us2];
}

/**
 * @brief Compare two strings, ignoring case.
 * @param s1 String 1.
 * @param s2 String 2.
 * @param n Number of characters to compare.
 * @return An integer greater than, equal to or less than 0 according to whether
 *         \p s1 is greater than, equal to or less than \p s2.
 *
 * Lexicographically compare the two strings with a maximum of \p n
 * comparisons.
 */
int strnicmp(const char *s1, const char *s2, int n)
{
	const uint8_t *map = charmap,
	              *us1 = (uint8_t*)s1,
		      *us2 = (uint8_t*)s2;

	if(n == 0)
		return 0;

	do {
		if(map[*us1] != map[*us2++])
			return map[*us1] - map[*--us2];

		if(*us1++ == '\0')
			break;
	} while(--n);

	return 0;

}

/** @} */

