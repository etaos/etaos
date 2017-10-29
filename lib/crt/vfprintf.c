/*
 *  ETA/OS - vfprintf
 *  Copyright (C) 2014   Michel Megens
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

/** @file vfprintf.c */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/types.h>
#include <etaos/preempt.h>

/**
 * @addtogroup libcio
 * @{
 */

#define BUFF 16
#define FLT_DIGITS 6

static int convert_to_num(uint64_t num, uint8_t base, bool sign,
							  bool caps, struct file * stream)
{
	char buff[BUFF];
	char *cp;
	char *digx;
	//memset(cp, 0xa, BUFF-1UL);

	if(num == 0) {
		fputc('0', stream);
		return 0;
	} else if((int64_t)num < 0 && sign) {
		fputc('-', stream);
	}

	if(caps) {
		digx = "0123456789ABCDEF";
	} else {
		digx = "0123456789abcdef";
	}

	cp = buff + BUFF - 1UL;
	*cp = '\0';
	if(base == 16) {
		do {
			*--cp = digx[num & 0x0F];
			num >>= 4;
		} while(num);
	} else if(base == 10) {
		if(num < 10) {
			*--cp = (char)num + '0';
		} else {
			do {
				*--cp = (char)(num % 10) + '0';
				num /= 10;
			} while(num);
		}
	}

	fputs(cp, stream);
	return 0;
}

static void print_flt(double num, struct file * output)
{
	size_t int_part;
	double remainder;
	uint8_t digits = FLT_DIGITS;
	int printable;

	if(num < 0.0)
		fputc('-', output);

	/*
	 * Round the number
	 */
	double rounding = 0.5;
	uint8_t i;
	for(i = 0; i < FLT_DIGITS; i++)
		rounding /= 10.0;

	num += rounding;
	int_part = (size_t)num;
	remainder = num - (double)int_part;

	/*
	 * print the first part
	 */
	convert_to_num(int_part, 10, FALSE, FALSE, output);
	fputc('.', output);

	while(digits-- > 0) {
		remainder *= 10.0;
		printable = (int)remainder;
		convert_to_num(printable, 10, FALSE, FALSE, output);
		remainder -= printable;
	}
}

static const char *vfprintf_long_int(va_list ap, const char *fmt,
		size_t fmt_i, struct file * stream)
{
	switch(fmt[fmt_i]) {
		case 'l':
			fmt++;
			if(fmt[fmt_i] == 'i' || fmt[fmt_i] == 'd')
				convert_to_num(va_arg(ap, unsigned long long),
					10, true, false, stream);
			else
				convert_to_num(va_arg(ap, unsigned long long),
					10, false, false, stream);
			break;

		case 'i':
		case 'd':
			convert_to_num(va_arg(ap, unsigned long), 10, true,
					false, stream);
			break;
		case 'u':
			convert_to_num(va_arg(ap, unsigned long), 10, false,
					false, stream);
			break;
	}

	return fmt;
}

/**
 * @brief Backend for fprintf, printf, etc
 * @param stream File to write to.
 * @param fmt Format string.
 * @param ap VA list to complete the format string.
 * @return Number of bytes written to \p stream.
 */
int vfprintf(struct file * stream, const char *fmt, va_list ap)
{
	size_t i;
	uint32_t uval;

	preempt_disable();
	for(i = 0; fmt[i] != '\0'; i++) {
		if(fmt[i] != '%') {
			fputc(fmt[i], stream);
		} else {
			fmt++;
			switch(fmt[i]) {
#if defined(PRINTF_FLT)
#warning "vfprintf does NOT support floating points yet!"
#endif
			case 'l':
				fmt++;
				fmt =  vfprintf_long_int(ap, fmt, i, stream);
				break;

			case 'd':
			case 'i':
				convert_to_num(va_arg(ap, unsigned int), 10, true, false, stream);
				break;

			case 'u':
				convert_to_num(va_arg(ap, unsigned int), 10, false, false, stream);
				break;

			case 'x':
				convert_to_num(va_arg(ap, unsigned int), 16, false, false, stream);
				break;

			case 'f':
				print_flt(va_arg(ap, double), stream);
				break;

			case 'p':
			case 'X':
				if(fmt[i] == 'p')
					uval = (size_t)va_arg(ap, void*);
				else
					uval = (uint32_t)va_arg(ap, unsigned int);

				convert_to_num(uval, 16, false, true, stream);
				break;

			case 'c':
				fputc((char)va_arg(ap, unsigned int), stream);
				break;

			case 's':
				fputs((char*)va_arg(ap, size_t), stream);
				break;

			case '%':
				fputc('%', stream);
				break;

			default:
				break; /* oeps */
			}
		}
	}
	preempt_enable();

	return stream->length;
}

/** @} */
