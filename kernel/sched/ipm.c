/*
 *  ETA/OS - Inter-Process Messages
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

#include <etaos/kernel.h>
#include <etaos/thread.h>
#include <etaos/types.h>
#include <etaos/ipm.h>
#include <etaos/mem.h>
#include <etaos/evm.h>

void ipm_queue_init(struct ipm_queue *iq, size_t len)
{
	void *ptr;

	thread_queue_init(&iq->qp);
	ptr = kzalloc(sizeof(*iq->msgs) * len);
	if(!ptr)
		return;

	iq->msgs = ptr;
	iq->num = len;
	iq->wr_idx = 0;
	iq->rd_idx = 0;
}

void ipm_post_msg(struct ipm_queue *iq, const void *buff, size_t len)
{
	unsigned long flags;
	struct ipm *msg;

	spin_lock_irqsave(&iq->lock, flags);
	msg = &iq->msgs[iq->wr_idx];
	iq->wr_idx += 1;
	spin_unlock_irqrestore(&iq->lock, flags);

	msg->data = buff;
	msg->len = len;

	evm_signal_event_queue(&iq->qp);
}

void ipm_get_msg(struct ipm_queue *iq, struct ipm *msg)
{
	unsigned long flags;

	evm_wait_event_queue(&iq->qp, EVM_WAIT_INFINITE);

	spin_lock_irqsave(&iq->lock, flags);
	if(iq->wr_idx == iq->rd_idx)
		return;

	*msg = iq->msgs[iq->rd_idx];
	iq->rd_idx += 1;

	if(iq->rd_idx != iq->wr_idx) {
		spin_unlock_irqrestore(&iq->lock, flags);
		evm_signal_event_queue(&iq->qp);
		spin_lock_irqsave(&iq->lock, flags);
	}

	spin_unlock_irqrestore(&iq->lock, flags);
	return;
}

bool ipm_reset_queue(struct ipm_queue *iq)
{
	unsigned long flags;

	spin_lock_irqsave(&iq->lock, flags);
	if(iq->qp.qhead != SIGNALED && iq->qp.qhead != NULL) {
		spin_unlock_irqrestore(&iq->lock, flags);
		return false;
	}

	iq->qp.qhead = NULL;
	iq->wr_idx = 0;
	iq->rd_idx = 0;
	spin_unlock_irqrestore(&iq->lock, flags);

	return true;
}

