/*
 *  ETA/OS - Init
 *  Copyright (C) 2014   Michel Megens
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

#ifndef __INIT_H__
#define __INIT_H__

#include <etaos/compiler.h>
#include <asm/init.h>

#ifndef SUBSYS_ATTRIB
#define SUBSYS_ATTRIB
#endif

#ifndef SUBSYS_INIT_CALL
#define SUBSYS_INIT_CALL
#endif

#ifndef MOD_ATTRIB
#define MOD_ATTRIB
#endif

#ifndef MOD_INIT_CALL
#define MOD_INIT_CALL
#endif

#ifndef DEV_ATTRIB
#define DEV_ATTRIB
#endif

#ifndef DEV_INIT_CALL
#define DEV_INIT_CALL
#endif

CDECL
#ifdef CONFIG_SCHED
extern void main_thread_func(void *arg);
#else
extern void main_init(void);
#endif
CDECL_END

/**
 * @ingroup kern-init
 * @brief Automated subsytem init.
 * @param ss_init_fn Init functions.
 */
#define subsys_init(ss_init_fn) \
	static __used SUBSYS_ATTRIB void __ ## ss_init_fn(void) \
	{ \
		SUBSYS_INIT_CALL(ss_init_fn); \
	}

/**
 * @ingroup kern-init
 * @brief Automated module init.
 * @param ss_init_fn Init functions.
 */
#define module_init(ss_init_fn) \
	static __used MOD_ATTRIB void __ ## ss_init_fn(void) \
	{ \
		MOD_INIT_CALL(ss_init_fn); \
	}

/**
 * @ingroup kern-init
 * @brief Automated device init.
 * @param ss_init_fn Init functions.
 */
#define device_init(ss_init_fn) \
	static __used DEV_ATTRIB void __ ## ss_init_fn(void) \
	{ \
		DEV_INIT_CALL(ss_init_fn); \
	}

#endif /* __INIT_H__ */

