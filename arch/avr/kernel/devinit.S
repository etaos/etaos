/*
 *  Eta/OS - AVR5 device init
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

#include <asm/config.h>
#include <asm/io.h>

.section .devinit0
.global dev_init
.func dev_init
dev_init:
#if defined(CONFIG_DRIVER_CORE) || defined(CONFIG_DRIVER_CORE_MODULE)
	call dev_core_init
#else
	mov r1, r1
#endif
.endfunc

.section .devinit2
.func dev_init_exit
dev_init_exit:
	ret
.endfunc

