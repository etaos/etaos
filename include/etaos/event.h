/*
 *  ETA/OS - Event management
 *  Copyright (C) 2015   Michel Megens
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
 * @file etaos/evm.h
 */

/**
 * @addtogroup evm
 */
/* @{ */

#ifndef __EVM_HEADER_H__
#define __EVM_HEADER_H__

struct thread;
struct thread_queue;

CDECL
extern int event_wait(struct thread_queue *qp, unsigned ms);
extern int raw_event_wait(struct thread_queue *qp, unsigned ms);
extern void event_notify(struct thread_queue *qp);
extern void event_notify_irq(struct thread_queue *qp);

#define evm_signal_event_queue(__qp) event_notify(__qp)
#define evm_signal_from_irq(__qp) event_notify_irq(__qp)
#define evm_wait_next_event_queue(__qp, __ms) event_wait(__qp, __ms)
#define evm_wait_event_queue(__qp, __ms) raw_event_wait(__qp, __ms)

CDECL_END
/**
 * @brief Put the thread in in an infinite wait for an event.
 */
#define EVM_WAIT_INFINITE EVENT_WAIT_INFINITE

#define EVENT_WAIT_INFINITE 0

#endif

/* @} */

