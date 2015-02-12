/*
 *  ETA/OS - LibC mem copy
 *  Copyright (C) 2004 by Egnite Software GmbH
 *  Copyright (C) 2015  Michel Megens <dev@michelmegens.net>
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
#include <etaos/types.h>
#include <etaos/stdlib.h>

typedef long word;      /* "word" used for optimal copy speed */

#define wsize   sizeof(word)
#define wmask   (wsize - 1)

/**
 * @brief Copy a memory region to another region.
 * @param dst0 Destination region.
 * @param src0 Source region.
 * @param length Number of bytes to copy.
 */
void *memcpy(void *dst0, const void *src0, size_t length)
{
	register char *dst = dst0;
	register const char *src = src0;
	register size_t t;

	if (length == 0 || dst == src)      /* nothing to do */
		goto done;

	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#define TLOOP(s) if (t) TLOOP1(s)
#define TLOOP1(s) do { s; } while (--t)

	if(dst < src) {
		t = (size_t)src;  /* only need low bits */
		if ((t | (size_t)dst) & wmask) {
			if ((t ^ (size_t)dst) & wmask || length < wsize)
				t = length;
			else
				t = wsize - (t & wmask);
			length -= t;
			TLOOP1(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / wsize;
		TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
		t = length & wmask;
		TLOOP(*dst++ = *src++);
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (size_t)src;
		if ((t | (size_t)dst) & wmask) {
			if ((t ^ (size_t)dst) & wmask || length <= wsize)
				t = length;
			else
				t &= wmask;
			length -= t;
			TLOOP1(*--dst = *--src);
		}

		t = length / wsize;
		TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
		t = length & wmask;
		TLOOP(*--dst = *--src);
	}

done:
	return dst0;
}

/** @} */

