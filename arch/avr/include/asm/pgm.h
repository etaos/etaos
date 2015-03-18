/*
 *  ETA/OS - ROM I/O
 *  Copyright (C) 2012, 2014, 2015   AVR-LibC, Michel Megens
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

#ifndef __AVR_PGM_H
#define __AVR_PGM_H

#define __pgm __attribute__((__progmem__))

# define PSTR(s) (__extension__({static const char __c[] __pgm = (s); &__c[0];}))

#define __LPM_classic__(addr)   \
	(__extension__({                \
	uint16_t __addr16 = (uint16_t)(addr); \
	uint8_t __result;           \
	__asm__                     \
	(                           \
	"lpm" "\n\t"            \
	"mov %0, r0" "\n\t"     \
	: "=r" (__result)       \
	: "z" (__addr16)        \
	: "r0"                  \
	);                          \
	__result;                   \
}))

#define __LPM_enhanced__(addr)  \
	(__extension__({                \
	uint16_t __addr16 = (uint16_t)(addr); \
	uint8_t __result;           \
	__asm__                     \
	(                           \
	"lpm %0, Z" "\n\t"      \
	: "=r" (__result)       \
	: "z" (__addr16)        \
	);                          \
	__result;                   \
}))

#define __LPM_word_classic__(addr)          \
	(__extension__({                            \
	uint16_t __addr16 = (uint16_t)(addr);   \
	uint16_t __result;                      \
	__asm__                                 \
	(                                       \
	"lpm"           "\n\t"              \
	"mov %A0, r0"   "\n\t"              \
	"adiw r30, 1"   "\n\t"              \
	"lpm"           "\n\t"              \
	"mov %B0, r0"   "\n\t"              \
	: "=r" (__result), "=z" (__addr16)  \
	: "1" (__addr16)                    \
	: "r0"                              \
	);                                      \
	__result;                               \
}))

#define __LPM_word_enhanced__(addr)         \
	(__extension__({                            \
	uint16_t __addr16 = (uint16_t)(addr);   \
	uint16_t __result;                      \
	__asm__                                 \
	(                                       \
	"lpm %A0, Z+"   "\n\t"              \
	"lpm %B0, Z"    "\n\t"              \
	: "=r" (__result), "=z" (__addr16)  \
	: "1" (__addr16)                    \
	);                                      \
	__result;                               \
}))

#define __LPM_dword_classic__(addr)         \
	(__extension__({                            \
	uint16_t __addr16 = (uint16_t)(addr);   \
	uint32_t __result;                      \
	__asm__                                 \
	(                                       \
	"lpm"           "\n\t"              \
	"mov %A0, r0"   "\n\t"              \
	"adiw r30, 1"   "\n\t"              \
	"lpm"           "\n\t"              \
	"mov %B0, r0"   "\n\t"              \
	"adiw r30, 1"   "\n\t"              \
	"lpm"           "\n\t"              \
	"mov %C0, r0"   "\n\t"              \
	"adiw r30, 1"   "\n\t"              \
	"lpm"           "\n\t"              \
	"mov %D0, r0"   "\n\t"              \
	: "=r" (__result), "=z" (__addr16)  \
	: "1" (__addr16)                    \
	: "r0"                              \
	);                                      \
	__result;                               \
}))

#define __LPM_dword_enhanced__(addr)        \
	(__extension__({                            \
	uint16_t __addr16 = (uint16_t)(addr);   \
	uint32_t __result;                      \
	__asm__                                 \
	(                                       \
	"lpm %A0, Z+"   "\n\t"              \
	"lpm %B0, Z+"   "\n\t"              \
	"lpm %C0, Z+"   "\n\t"              \
	"lpm %D0, Z"    "\n\t"              \
	: "=r" (__result), "=z" (__addr16)  \
	: "1" (__addr16)                    \
	);                                      \
	__result;                               \
}))

#define __LPM_float_classic__(addr)         \
	(__extension__({                            \
	uint16_t __addr16 = (uint16_t)(addr);   \
	float __result;                         \
	__asm__                                 \
	(                                       \
	"lpm"           "\n\t"              \
	"mov %A0, r0"   "\n\t"              \
	"adiw r30, 1"   "\n\t"              \
	"lpm"           "\n\t"              \
	"mov %B0, r0"   "\n\t"              \
	"adiw r30, 1"   "\n\t"              \
	"lpm"           "\n\t"              \
	"mov %C0, r0"   "\n\t"              \
	"adiw r30, 1"   "\n\t"              \
	"lpm"           "\n\t"              \
	"mov %D0, r0"   "\n\t"              \
	: "=r" (__result), "=z" (__addr16)  \
	: "1" (__addr16)                    \
	: "r0"                              \
	);                                      \
	__result;                               \
}))

#define __LPM_float_enhanced__(addr)        \
	(__extension__({                            \
	uint16_t __addr16 = (uint16_t)(addr);   \
	float __result;                         \
	__asm__                                 \
	(                                       \
	"lpm %A0, Z+"   "\n\t"              \
	"lpm %B0, Z+"   "\n\t"              \
	"lpm %C0, Z+"   "\n\t"              \
	"lpm %D0, Z"    "\n\t"              \
	: "=r" (__result), "=z" (__addr16)  \
	: "1" (__addr16)                    \
	);                                      \
	__result;                               \
}))

#if defined (__AVR_HAVE_LPMX__)
#define __LPM(addr)         __LPM_enhanced__(addr)
#define __LPM_word(addr)    __LPM_word_enhanced__(addr)
#define __LPM_dword(addr)   __LPM_dword_enhanced__(addr)
#define __LPM_float(addr)   __LPM_float_enhanced__(addr)
#else
#define __LPM(addr)         __LPM_classic__(addr)
#define __LPM_word(addr)    __LPM_word_classic__(addr)
#define __LPM_dword(addr)   __LPM_dword_classic__(addr)
#define __LPM_float(addr)   __LPM_float_classic__(addr)
#endif

#define pgm_read_byte_near(address_short) __LPM((uint16_t)(address_short))
#define pgm_read_word_near(address_short) __LPM_word((uint16_t)(address_short))
#define pgm_read_byte(address_short)    pgm_read_byte_near(address_short)
#define pgm_read_word(address_short)    pgm_read_word_near(address_short)

#ifdef __DOXYGEN__
/**
 * @ingroup atmega
 * @brief Program memory character.
 * @deprecated The use of GNU attributes in typedefs is deprecated.
 */
typedef char prog_char;
#else
typedef char __attribute__((__progmem__)) prog_char;
#endif

/**
 * @ingroup atmega
 * @brief Copy memory from program memroy in data memory.
 * @param _dest Destination addess (data space address).
 * @param _src Source address (program space address).
 * @param size Number bytes to copy.
 * @return Destination address pointer.
 */
static inline void *memcpy_P(void *_dest, const void *_src, size_t size)
{
	size_t idx;
	char *dest = _dest;
	const char *src = _src;

	for(idx = 0; idx < size; idx++)
		dest[idx] = pgm_read_byte(src+idx);

	return (void*)dest;
}

static inline size_t strlen_P(const char *s)
{
	const char *c;

	for(c = s; pgm_read_byte(c); ++c);
	return (size_t)(c - s);
}

#endif
