/*
 *  ETA/OS - Call tracer
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
 * @addtogroup trace
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdlib.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/trace.h>
#include <etaos/init.h>

/**
 * @brief Store information into a trace info structure.
 * @param info Trace info structure.
 * @param file File name.
 * @param line Line number.
 * @param alloc Indicates whether or not file a pointer to stack variable.
 * @note If \p alloc is set to true the file name will be copied to a locally
 *       allocated buffer.
 */
void trace_info_set(trace_info_t *info, const char *file, int line, bool alloc)
{
#ifdef CONFIG_SCHED
	struct thread *tp;

	tp = current_thread();
	info->owner = tp;
#endif

	if(likely(post_early_init) && alloc) {
		info->file = strdup(file);
		info->allocated = true;
	} else {
		info->file = file;
		info->allocated = false;
	}

	info->line = line;

}

/**
 * @brief Destory information stored in a trace.
 * @param info Trace info structure to invalidate.
 */
void trace_info_invalidate(trace_info_t *info)
{
#ifdef CONFIG_SCHED
	info->owner = NULL;
#endif

	if(info->file && post_early_init) {
		if(info->allocated)
			kfree((void*)info->file);

		info->file = NULL;
	}

	info->line = -1;
	info->alloc = false;
}

/** @} */
