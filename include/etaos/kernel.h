/*
 *  Eta/OS - ETA/OS kernel header
 *  Copyright (C) 2014, 2015, 2016   Michel Megens <dev@bietje.net>
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
 * @file etaos/kernel.h
 */

/**
 * @addtogroup kernel
 * @{
 */

#ifndef __ETAOS_KERNEL_H__
#define __ETAOS_KERNEL_H__

#include <generated/autoconf.h>
#include <etaos/types.h>
#include <etaos/stddef.h>
#include <etaos/compiler.h>

#ifdef __cplusplus
#define NULL (0)
#define SIGNALED ((void*)-1)
#define ERR_PTR (-1)
#else
#define NULL ((void*)0)
#define SIGNALED ((void*)-1)
#define ERR_PTR ((void*)-1)
#endif

#define USED(__x__) ((void)__x__)

#define FALSE 0
#define TRUE !FALSE

#define true TRUE
#define false FALSE

#ifdef __compiler_offsetof
/**
 * @brief Calculate the offset of a member in a structure.
 * @param TYPE Type of the member.
 * @param MEMBER Member to calculate the offset of.
 */
#define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * @brief Get the container of a pointer member.
 * @param ptr Pointer to get the container of.
 * @param type Type of the container.
 * @param member Member name of \p ptr in the container.
 */
#define container_of(ptr, type, member) ({		\
		const typeof( ((type *)0)->member) *__mptr = (ptr); \
		(type *)( ( char *)__mptr - offsetof(type,member) );})

#ifdef __cplusplus

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual(void);

#define CDECL extern "C" {
#define CDECL_END }
#else
#define CDECL
#define CDECL_END
#endif

#define DEBUG_ON(__condi__) \
	(__condi__) ? fprintf_P(stderr, PSTR("[ASSERTION FAILED] %s:%i in %s: %s\n"), \
			__FILE__, __LINE__, __func__, #__condi__) : (void)0

#define DEBUG_ON_STR(__condi__, msg) \
	(__condi__) ? fprintf_P(stderr, PSTR("[ASSERTION FAILED]: %s - %s:%i in %s: %s\n"), \
			msg, __FILE__, __LINE__, __func__, #__condi__) : (void)0

/**
 * @brief sysctl settings type.
 */
typedef enum sys_ctl {
	/**
	 * @brief Set the stdout stream.
	 * @note Second argument to sysctl needs to be the stream.
	 */
	SYS_SET_STDOUT,

	/**
	 * @brief Set the stdout stream.
	 * @note Second argument to sysctl needs to be the stream.
	 */
	SYS_SET_STDERR,
	
	/**
	 * @brief Set the stdout stream.
	 * @note Second argument to sysctl needs to be the stream.
	 */
	SYS_SET_STDIN,

	SYS_SET_SYSCLK,
	SYS_SET_HR_CLK,

} sys_ctl_t;

CDECL
extern int sysctl(sys_ctl_t ctl, ...);
CDECL_END

#endif

/** @} */
