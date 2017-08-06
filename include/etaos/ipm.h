/*
 *  ETA/OS - Inter-Process Messages
 *  Copyright (C) 2014, 2015, 2016, 2017   Michel Megens
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
 * @file etaos/ipm.h
 */

/**
 * @addtogroup ipm
 */
/* @{ */

#ifndef __IPM_H__
#define __IPM_H__

#include <etaos/kernel.h>
#include <etaos/thread.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>

/**
 * @struct ipm
 * @brief IPM message structure
 */
struct ipm {
	const void *data; //!< Message data.
	size_t len; //!< Message data length.
};

/**
 * @struct ipm_queue
 * @brief IPM waiting queue.
 */
struct ipm_queue {
	struct thread_queue qp; //!< Thread queue to wait in.
	uint8_t num; //!< Length of the message array.
	uint8_t wr_idx; //!< Write index.
	uint8_t rd_idx; //!< Read index.
	struct ipm *msgs; //!< Message array.
	spinlock_t lock; //!< Concurrency lock.
};

CDECL

extern void ipm_queue_init(struct ipm_queue *qp, size_t len);
extern int ipm_post_msg(struct ipm_queue *qp, const void *data, size_t len);
extern void ipm_get_msg(struct ipm_queue *qp, struct ipm *msg);
extern bool ipm_reset_queue(struct ipm_queue *iq);

CDECL_END
#endif

/* @} */

