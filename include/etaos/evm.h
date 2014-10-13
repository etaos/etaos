/*
 *  ETA/OS - Event driven mutexes
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

#ifndef __EVM_HEADER_H__
#define __EVM_HEADER_H__

struct thread;
struct thread_queue;

extern void evm_signal_event_queue(struct thread_queue *qp);
extern int evm_wait_event_queue(struct thread_queue *qp, unsigned ms);
extern void evm_signal_from_irq(struct thread_queue *qp);
extern int evm_wait_next_event_queue(struct thread_queue *qp, unsigned ms);

#endif
