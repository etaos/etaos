/*
 *  ETA/OS - Python platform
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

#undef __FILE_ID__
#define __FILE_ID__ 0x70

#include <etaos/kernel.h>
#include <etaos/string.h>
#include <etaos/types.h>
#include <etaos/python.h>
#include <etaos/irq.h>
#include <etaos/stdio.h>
#include <etaos/hrtimer.h>

#include <asm/pgm.h>

#define PYTHON_SYSTEM_TICK_INTERVAL 1000000	/* 1 milisecond */

static struct hrtimer *timer;
static uint32_t interval = 1000000;

static void platform_timer_handle(struct hrtimer *hrt, void *arg)
{
	uint32_t interval;
	uint32_t *intervalptr = arg;

	interval = *intervalptr;
	pm_vmPeriodic(interval / 1000);
}

PmReturn_t plat_init(void)
{
	hrtimer_create(hr_sys_clk, PYTHON_SYSTEM_TICK_INTERVAL,
		       &platform_timer_handle, &interval, 0UL);
	return PM_RET_OK;
}

PmReturn_t plat_deinit(void)
{
	hrtimer_stop(timer);
	return PM_RET_OK;
}

uint8_t plat_memGetByte(PmMemSpace_t memspace, uint8_t const **paddr)
{
	uint8_t b = 0;

	switch (memspace) {
	case MEMSPACE_RAM:
		b = **paddr;
		*paddr += 1;
		return b;

	case MEMSPACE_PROG:
		b = pgm_read_byte(*paddr);
		*paddr += 1;
		return b;

	case MEMSPACE_EEPROM:
		return 0;
		/*b = eeprom_read_byte(*paddr);
		 *paddr += 1;
		 return b;*/

	case MEMSPACE_SEEPROM:
	case MEMSPACE_OTHER0:
	case MEMSPACE_OTHER1:
	case MEMSPACE_OTHER2:
	case MEMSPACE_OTHER3:
	default:
		return 0;
	}
}

PmReturn_t plat_putByte(uint8_t b)
{
	int byte = b;

	fputc(byte, stdout);
	return PM_RET_OK;
}

PmReturn_t plat_getByte(uint8_t * b)
{
	int rv;

	rv = fgetc(stdin);
	*b = (uint8_t) rv;
	return PM_RET_OK;
}

PmReturn_t plat_getMsTicks(uint32_t * r_ticks)
{
	unsigned long flags;

	irq_save_and_disable(&flags);
	*r_ticks = pm_timerMsTicks;
	irq_restore(&flags);

	return PM_RET_OK;
}

#ifdef HAVE_DEBUG_INFO
#define LEN_FNLOOKUP 29
#define LEN_EXNLOOKUP 18
#define FN_MAX_LEN 64
#define EXN_MAX_LEN 64
#ifndef MAX
#define MAX(a,b) (((a) >= (b)) ? (a) : (b))
#endif

/* This table should match src/vm/fileid.txt */
static const char fnstr_00[] __pgm = "<no file>";
static const char fnstr_01[] __pgm = "codeobj.c";
static const char fnstr_02[] __pgm = "dict.c";
static const char fnstr_03[] __pgm = "frame.c";
static const char fnstr_04[] __pgm = "func.c";
static const char fnstr_05[] __pgm = "global.c";
static const char fnstr_06[] __pgm = "heap.c";
static const char fnstr_07[] __pgm = "img.c";
static const char fnstr_08[] __pgm = "int.c";
static const char fnstr_09[] __pgm = "interp.c";
static const char fnstr_10[] __pgm = "pmstdlib_nat.c";
static const char fnstr_11[] __pgm = "list.c";
static const char fnstr_12[] __pgm = "main.c";
static const char fnstr_13[] __pgm = "mem.c";
static const char fnstr_14[] __pgm = "module.c";
static const char fnstr_15[] __pgm = "obj.c";
static const char fnstr_16[] __pgm = "seglist.c";
static const char fnstr_17[] __pgm = "sli.c";
static const char fnstr_18[] __pgm = "strobj.c";
static const char fnstr_19[] __pgm = "tuple.c";
static const char fnstr_20[] __pgm = "seq.c";
static const char fnstr_21[] __pgm = "pm.c";
static const char fnstr_22[] __pgm = "thread.c";
static const char fnstr_23[] __pgm = "float.c";
static const char fnstr_24[] __pgm = "class.c";
static const char fnstr_25[] __pgm = "bytearray.c";
static const char fnstr_26[] __pgm = "cpu.c";
static const char fnstr_27[] __pgm = "eeprom.c";
static const char fnstr_28[] __pgm = "sram.c";

static const char *const fnlookup[LEN_FNLOOKUP] __pgm = {
	fnstr_00, fnstr_01, fnstr_02, fnstr_03,
	fnstr_04, fnstr_05, fnstr_06, fnstr_07,
	fnstr_08, fnstr_09, fnstr_10, fnstr_11,
	fnstr_12, fnstr_13, fnstr_14, fnstr_15,
	fnstr_16, fnstr_17, fnstr_18, fnstr_19,
	fnstr_20, fnstr_21, fnstr_22, fnstr_23,
	fnstr_24, fnstr_25, fnstr_26
};

/* This table should match src/vm/pm.h PmReturn_t */
static const char exnstr_00[] __pgm = "Exception";
static const char exnstr_01[] __pgm = "SystemExit";
static const char exnstr_02[] __pgm = "IoError";
static const char exnstr_03[] __pgm = "ZeroDivisionError";
static const char exnstr_04[] __pgm = "AssertionError";
static const char exnstr_05[] __pgm = "AttributeError";
static const char exnstr_06[] __pgm = "ImportError";
static const char exnstr_07[] __pgm = "IndexError";
static const char exnstr_08[] __pgm = "KeyError";
static const char exnstr_09[] __pgm = "MemoryError";
static const char exnstr_10[] __pgm = "NameError";
static const char exnstr_11[] __pgm = "SyntaxError";
static const char exnstr_12[] __pgm = "SystemError";
static const char exnstr_13[] __pgm = "TypeError";
static const char exnstr_14[] __pgm = "ValueError";
static const char exnstr_15[] __pgm = "StopIteration";
static const char exnstr_16[] __pgm = "Warning";

static const char *const exnlookup[LEN_EXNLOOKUP] __pgm = {
	exnstr_00, exnstr_01, exnstr_02, exnstr_03,
	exnstr_04, exnstr_05, exnstr_06, exnstr_07,
	exnstr_08, exnstr_09, exnstr_10, exnstr_11,
	exnstr_12, exnstr_13, exnstr_14, exnstr_15,
	exnstr_16
};
#endif				/* HAVE_DEBUG_INFO */

void plat_reportError(PmReturn_t result)
{
#ifdef HAVE_DEBUG_INFO
	uint8_t res;
	pPmFrame_t pframe;
	pPmObj_t pstr;
	PmReturn_t retval;
	uint16_t bcindex;
	uint16_t bcsum;
	uint16_t linesum;
	uint16_t len_lnotab;
	uint8_t const *plnotab;
	uint16_t i;
	char pstrbuf[MAX(FN_MAX_LEN, EXN_MAX_LEN)];

	/* Print traceback */
	puts_P(PSTR("Traceback (most recent call first):"));

	/* Get the top frame */
	pframe = gVmGlobal.pthread->pframe;

	/* If it's the native frame, print the native function name */
	if (pframe == (pPmFrame_t) & (gVmGlobal.nativeframe)) {

		/* The last name in the names tuple of the code obj is the name */
		retval =
		    tuple_getItem((pPmObj_t) gVmGlobal.nativeframe.
				  nf_func->f_co->co_names, -1, &pstr);
		if ((retval) != PM_RET_OK) {
			puts_P(PSTR("  Unable to get native func name."));
			return;
		} else {
			printf_P(PSTR("  %s() __NATIVE__\n"),
				 ((pPmString_t) pstr)->val);
		}

		/* Get the frame that called the native frame */
		pframe = (pPmFrame_t) gVmGlobal.nativeframe.nf_back;
	}

	/* Print the remaining frame stack */
	for (; pframe != C_NULL; pframe = pframe->fo_back) {
		/* The last name in the names tuple of the code obj is the name */
		retval =
		    tuple_getItem((pPmObj_t) pframe->fo_func->f_co->co_names,
				  -1, &pstr);
		if ((retval) != PM_RET_OK)
			break;

		/*
		 * Get the line number of the current bytecode. Algorithm comes from:
		 * http://svn.python.org/view/python/trunk/Objects/lnotab_notes.txt?view=markup
		 */
		bcindex = pframe->fo_ip - pframe->fo_func->f_co->co_codeaddr;
		plnotab = pframe->fo_func->f_co->co_lnotab;
		len_lnotab = mem_getWord(MEMSPACE_PROG, &plnotab);
		bcsum = 0;
		linesum = pframe->fo_func->f_co->co_firstlineno;
		for (i = 0; i < len_lnotab; i += 2) {
			bcsum += mem_getByte(MEMSPACE_PROG, &plnotab);
			if (bcsum > bcindex)
				break;
			linesum += mem_getByte(MEMSPACE_PROG, &plnotab);
		}

		/* Get the file name of this frame's function */
		if (pframe->fo_func->f_co->co_memspace == MEMSPACE_PROG) {
			strncpy_P(pstrbuf,
				  (char *)pframe->fo_func->f_co->co_filename,
				  MAX(FN_MAX_LEN, EXN_MAX_LEN));
		}
		printf_P(PSTR("  File \"%s\", line %d, in %s\n"),
			 ((pframe->fo_func->f_co->co_memspace == MEMSPACE_PROG)
			  ? pstrbuf
			  : (char *)pframe->fo_func->f_co->co_filename),
			 linesum, ((pPmString_t) pstr)->val);
	}

	/* Print error */
	res = (uint8_t) result;
	if ((res > 0) && ((res - PM_RET_EX) < LEN_EXNLOOKUP)) {
		strncpy_P(pstrbuf,
			  (const char *)
			  pgm_read_word(&exnlookup[res - PM_RET_EX]),
			  EXN_MAX_LEN);
		printf_P(PSTR("%s"), pstrbuf);
	} else {
		printf_P(PSTR("Error code 0x%02X"), result);
	}
	printf_P(PSTR(" detected by "));

	if ((gVmGlobal.errFileId > 0) && (gVmGlobal.errFileId < LEN_FNLOOKUP)) {
		strncpy_P(pstrbuf,
			  (const char *)
			  pgm_read_word(&fnlookup[gVmGlobal.errFileId]),
			  FN_MAX_LEN);
		printf_P(PSTR("%s:"), pstrbuf);
	} else {
		printf_P(PSTR("FileId 0x%02X line "), gVmGlobal.errFileId);
	}
	printf_P(PSTR("%d\n"), gVmGlobal.errLineNum);

#else				/* HAVE_DEBUG_INFO */

	/* Print error */
	printf_P(PSTR("Error:     0x%02X\n"), result);
	printf_P(PSTR("  Release: 0x%02X\n"), gVmGlobal.errVmRelease);
	printf_P(PSTR("  FileId:  0x%02X\n"), gVmGlobal.errFileId);
	printf_P(PSTR("  LineNum: %d\n"), gVmGlobal.errLineNum);

	/* Print traceback */
	{
		pPmObj_t pframe;
		pPmObj_t pstr;
		PmReturn_t retval;

		puts_P(PSTR("Traceback (top first):"));

		/* Get the top frame */
		pframe = (pPmObj_t) gVmGlobal.pthread->pframe;

		/* If it's the native frame, print the native function name */
		if (pframe == (pPmObj_t) & (gVmGlobal.nativeframe)) {

			/* The last name in the names tuple of the code obj is the name */
			retval =
			    tuple_getItem((pPmObj_t) gVmGlobal.nativeframe.
					  nf_func->f_co->co_names, -1, &pstr);
			if ((retval) != PM_RET_OK) {
				puts_P(PSTR
				       ("  Unable to get native func name."));
				return;
			} else {
				printf_P(PSTR("  %s() __NATIVE__\n"),
					 ((pPmString_t) pstr)->val);
			}

			/* Get the frame that called the native frame */
			pframe = (pPmObj_t) gVmGlobal.nativeframe.nf_back;
		}

		/* Print the remaining frame stack */
		for (;
		     pframe != C_NULL;
		     pframe = (pPmObj_t) ((pPmFrame_t) pframe)->fo_back) {
			/* The last name in the names tuple of the code obj is the name */
			retval =
			    tuple_getItem((pPmObj_t)
					  ((pPmFrame_t) pframe)->fo_func->f_co->
					  co_names, -1, &pstr);
			if ((retval) != PM_RET_OK)
				break;

			printf_P(PSTR("  %s()\n"), ((pPmString_t) pstr)->val);
		}
		puts_P(PSTR("  <module>."));
	}
#endif				/* HAVE_DEBUG_INFO */
}
