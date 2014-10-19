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

#ifndef __IPM_H__
#define __IPM_H__

#include <etaos/kernel.h>
#include <etaos/thread.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>

struct ipm {
	void *data;
	size_t len;
};

struct ipm_queue {
	struct thread_queue qp;
	uint8_t num;
	uint8_t wr_idx, rd_idx;
	struct ipm *msgs;
	spinlock_t lock;
};

extern void ipm_queue_init(struct ipm_queue *qp, size_t len);
extern void ipm_post_msg(struct ipm_queue *qp, void *data, size_t len);
extern void ipm_get_msg(struct ipm_queue *qp, struct ipm *msg);

#endif
