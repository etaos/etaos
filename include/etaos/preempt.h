/*
 *  ETA/OS - Preemption
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

#ifndef __PREEMPT_H__
#define __PREEMPT_H__

struct thread;
extern void schedule(void);

#ifdef CONFIG_PREEMPT
extern void __preempt_add(int num);
extern void __preempt_sub(int num);
extern int *preempt_counter_ptr(void);
extern bool should_resched(void);

#define preempt_count_inc __preempt_add(1)
#define preempt_count_dec __preempt_sub(1)

static inline int preempt_dec_and_test(void)
{
	return !--*preempt_counter_ptr() && should_resched();
}

static inline int preempt_test(void)
{
	return !*preempt_counter_ptr();
}

static inline void preempt_enable(void)
{
	if(preempt_dec_and_test())
		schedule();
}

static inline void preempt_enable_no_resched(void)
{
	preempt_count_dec;
}

static inline void preempt_disable(void)
{
	preempt_count_inc;
}
#else /* !CONFIG_PREEMPT */

#define __preempt_add(_i)
#define __preempt_sub(_i)
#define preempt_counter_ptr()
#define preempt_count_inc
#define preempt_count_dec
#define preempt_dec_and_test()
#define preempt_enable()
#define preempt_enable_no_resched()
#define preempt_disable()
#define preempt_test() 1
#endif /* CONFIG_PREEMPT */
#endif /* __PREEMPT_H__ */

