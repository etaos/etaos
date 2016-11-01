/*
 *  Eta/OS - AVR arch boot
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

#ifndef __AVR_INIT_H__
#define __AVR_INIT_H__

#ifndef __ASSEMBLER__
CDECL
extern void sched_start();
CDECL_END

#define SUBSYS_ATTRIB __attribute__((section (".subsysinit"))) \
	__attribute__((naked))
#define MOD_ATTRIB __attribute__((section (".modinit"))) \
	__attribute__((naked))
#define DEV_ATTRIB __attribute__((section (".devinit1"))) \
	__attribute__((naked))
#define CHIP_ATTRIB __attribute__((section (".chipinit"))) \
	__attribute__((naked))

#define DEV_INIT_CONTENT(init_fn) \
	__asm__ __volatile__("call "  #init_fn ::: )

#define SUBSYS_INIT_CALL(init_fn) DEV_INIT_CONTENT(init_fn)
#define MOD_INIT_CALL(init_fn) DEV_INIT_CONTENT(init_fn)
#define DEV_INIT_CALL(init_fn) DEV_INIT_CONTENT(init_fn)
#define CHIP_INIT_CALL(init_fn) DEV_INIT_CONTENT(init_fn)

#ifdef CONFIG_SCHED
#define sys_init() sched_start();
#else
#define sys_init() main_init();
#endif

#endif /* __ASSEMBLER__ */
#endif /* __AVR_INIT_H__ */
