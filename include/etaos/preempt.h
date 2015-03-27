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

/**
 * @file etaos/preempt.h
 */

#ifndef __PREEMPT_H__
#define __PREEMPT_H__

/**
 * @addtogroup preempt
 */
/* @{ */
struct thread;

CDECL

extern void schedule(void);
extern bool preempt_should_resched(void);

#ifdef CONFIG_PREEMPT
/**
 * @brief Add to the preemption counter of the current thread.
 * @param num Number to add.
 */
extern void __preempt_add(int num);
/**
 * @brief Subtract from the preemption counter of the current thread.
 * @param num Number to substract.
 */
extern void __preempt_sub(int num);
/**
 * @brief Get the preemption counter pointer of the current thread.
 * @return The preemtion counter of the current thread.
 */
extern int *preempt_counter_ptr(void);
extern bool should_resched(void);

/**
 * @def preempt_count_inc
 * @brief Increase the preemption counter of the current thread by 1.
 */
#define preempt_count_inc __preempt_add(1)

/**
 * @def preempt_count_dec
 * @brief Decrease the preemption counter of the current thread by 1.
 */
#define preempt_count_dec __preempt_sub(1)

/**
 * @brief Decrease the preemption counter of the current thread and test it.
 * @retval true if preemption is enabled and the THREAD_NEED_RESCHED_FLAG
 *         is set.
 * @retval false in any other case.
 *
 * If the preemption counter turns out to be 0, preemption is enabled. If that
 * is the case, it doesn't immediatly mean that this function returns true.
 * For that, should_resched also has to return true.
 */
static inline int preempt_dec_and_test(void)
{
	return !--*preempt_counter_ptr() && preempt_should_resched();
}

/**
 * @brief Test if preemption is enabled.
 * @retval true if preemption is enabled.
 * @retval false if preemption is not enabled.
 */
static inline int preempt_test(void)
{
	return !*preempt_counter_ptr();
}

/**
 * @brief Enable preemption.
 * @note This function also reschedules if preempt_dec_and_test returns true.
 */
static inline void preempt_enable(void)
{
	if(preempt_dec_and_test())
		schedule();
}

/**
 * @brief Enable preemption, but do never reschedule.
 * @note Only use if you really have to (when you CANNOT reschedule), using
 *       preempt_enable results in a fairer and better system.
 *
 */
static inline void preempt_enable_no_resched(void)
{
	preempt_count_dec;
}

/**
 * @brief Disable preemption.
 */
static inline void preempt_disable(void)
{
	preempt_count_inc;
}

static inline int preempt_count(void)
{
	return *preempt_counter_ptr();
}

#define preemptible() ((preempt_count() == 0) && !irqs_disabled())

#else /* !CONFIG_PREEMPT */

#define preemptible() 0
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

CDECL_END

/* @} */
#endif /* __PREEMPT_H__ */

