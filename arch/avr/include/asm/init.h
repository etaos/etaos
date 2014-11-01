/*
 *  Eta/OS - AVR arch boot
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
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

#ifndef __AVR_INIT_H__
#define __AVR_INIT_H__

#ifndef __ASSEMBLER__
CDECL
extern void atmega_init_gpio();
extern void vfs_init();
extern void atmega_usart_init();
extern void avr_timer_init();
extern void avr_init_sched();
extern void sched_init();
extern void atmega_i2c_init(void);
CDECL_END

#define SUBSYS_ATTRIB __attribute__((section (".subsysinit"))) \
	__attribute__((naked))
#define MOD_ATTRIB __attribute__((section (".modinit"))) \
	__attribute__((naked))
#define DEV_ATTRIB __attribute__((section (".devinit1"))) \
	__attribute__((naked))

#define SUBSYS_INIT_CALL(init_fn) \
	__asm__ __volatile__("call "  #init_fn ::: )

#if defined(CONFIG_GPIO) || defined(CONFIG_GPIO_MODULE)
#define gpio_init() atmega_init_gpio()
#else
#define gpio_init()
#endif

#if defined(CONFIG_VFS) || defined(CONFIG_VFS_MODLE)
#define _vfs_init() vfs_init();
#else
#define _vfs_init()
#endif

#if defined(CONFIG_ATMEGA_USART) || defined(CONFIG_ATMEGA_USART_MODULE)
#define init_usart() atmega_usart_init();
#else
#define init_usart()
#endif

#if defined(CONFIG_I2C_ATMEGA) || defined(CONFIG_I2C_ATMEGA_MODULE)
#define i2c_init() atmega_i2c_init()
#else
#define i2c_init()
#endif

#ifdef CONFIG_TIMER
#define timer_init() avr_timer_init();
#else
#define timer_init()
#endif

#ifdef CONFIG_SCHED
#define sys_init() \
	avr_init_sched(); \
	sched_init();
#else
#define sys_init() main_init();
#endif

#endif /* __ASSEMBLER__ */
#endif /* __AVR_INIT_H__ */

