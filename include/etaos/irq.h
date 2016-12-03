/*
 *  ETA/OS - IRQ header
 *  Copyright (C) 2014   Michel Megens
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
 * @addtogroup irq
 * @{
 */
#ifndef __IRQ_H__
#define __IRQ_H__

#include <etaos/kernel.h>
#include <etaos/types.h>

struct irq_data;

CDECL

extern void irq_save_and_disable(unsigned long *flags);
extern void irq_restore(unsigned long *flags);

/* arch functions */
/**
 * @ingroup archAPI
 * @brief Setup the hardware registers for the IRQ.
 * @param data IRQ data structure.
 */
extern void cpu_request_irq(struct irq_data *data);
/**
 * @ingroup archAPI
 * @brief Restore the IRQ flags.
 * @param flags IRQ flags which have to be restored.
 */
extern void arch_irq_restore_flags(unsigned long *flags);
/**
 * @ingroup archAPI
 * @brief Get the IRQ flags.
 * @return IRQ flags.
 */
extern unsigned long arch_irq_get_flags();
/**
 * @ingroup archAPI
 * @brief Disable the global interrupts.
 */
extern void arch_irq_disable();
/**
 * @ingroup archAPI
 * @brief Enable the global interrupts.
 */
extern void arch_irq_enable();

/**
 * @ingroup archAPI
 * @brief Arch backend of local_irq_enable()
 */
extern void arch_local_irq_enable();

/**
 * @ingroup archAPI
 * @brief Arch backend of local_irq_disable()
 */
extern void arch_local_irq_disable();

/**
 * @ingroup archAPI
 * @brief Check if the interrupts are enabled.
 * @param flags Flags argument.
 *
 * The \p flags argument will tell which interrupts are enabled.
 */
extern void raw_irq_enabled_flags(unsigned long *flags);

/**
 * @brief Enable the IRQ's on the current CPU.
 */
static inline void local_irq_enable(void)
{
	arch_local_irq_enable();
}

/**
 * @brief Disable the IRQ's on the current CPU.
 */
static inline void local_irq_disable(void)
{
	arch_local_irq_disable();
}

/**
 * @brief Check whether the IRQ's are disabled or not.
 * @retval 1 The interrupts are disabled.
 * @retval 0 The interrupts are enabled.
 */
static inline bool irqs_disabled()
{
	unsigned long flags;

	raw_irq_enabled_flags(&flags);
	return flags == 0UL;
}

CDECL_END

/**
 * @brief Get the CPU IRQ chip.
 */
#define irq_get_chip() arch_get_irq_chip()

/**
 * @brief Enable interrupts
 */
#define irq_disable() arch_irq_disable()
/**
 * @brief Disable interrupts.
 */
#define irq_enable() arch_irq_enable()

struct irq_data;
struct irq_chip;

/**
 * @brief IRQ return type.
 */
typedef enum {
	IRQ_NONE = 0 << 0, //!< IRQ not handled.
	IRQ_HANDLED = 1 << 0, //!< IRQ handled.
	IRQ_WAKE_OWNER = 1 << 1, //!< Wake up the IRQ owner.
} irqreturn_t;

/**
 * @brief IRQ handler type.
 * @param irq IRQ the handle belongs to.
 * @param data Private data of the IRQ.
 */
typedef irqreturn_t (*irq_vector_t)(struct irq_data *irq, void *data);

/**
 * @brief Interrupt descriptor.
 */
struct irq_data {
	unsigned int irq; //!< IRQ vector number.
	struct list_head irq_list; //!< List of IRQ's.

	unsigned long flags; //!< IRQ flags.
	uint64_t num; //!< Number of triggers.
	struct irq_chip *chip; //!< IRQ chip.

	irq_vector_t handler; //!< IRQ handler.
	void *private_data; //!< IRQ private data.
};

#ifdef CONFIG_SCHED
/**
 * @brief Threaded IRQ data.
 */
struct irq_thread_data {
	struct thread *owner; //!< IRQ owner.
	struct irq_data idata; //!< IRQ data.
};
#endif

#define IRQ_ENABLE_FLAG 0 //!< Enable the IRQ.
#define IRQ_RISING_FLAG 1 //!< Rising edge IRQ.
#define IRQ_FALLING_FLAG 2 //!< Falling edge IRQ.
#define IRQ_THREADED_FLAG 3 //!< IRQ is handled by a thread.
#define IRQ_THREADED_TRIGGERED_FLAG 4 //!< Thread has been woken up.

/**
 * @brief Enable mask.
 */
#define IRQ_ENABLE_MASK (1 << IRQ_ENABLE_FLAG) 
/**
 * @brief Rising edge mask.
 */
#define IRQ_RISING_MASK (1 << IRQ_RISING_FLAG)
/**
 * @brief Falling edge mask.
 */
#define IRQ_FALLING_MASK (1 << IRQ_FALLING_FLAG)
/**
 * @brief Threaded IRQ mask.
 */
#define IRQ_THREADED_MASK (1 << IRQ_THREADED_FLAG)
/**
 * @brief Thread triggered mask.
 */
#define IRQ_THREADED_TRIGGERED_MASK (1 << IRQ_THREADED_TRIGGERED_FLAG)

/**
 * @brief IRQ chip descriptor.
 *
 * An IRQ chip is a chip which is capable of triggering IRQ's. For example,
 * the CPU.
 */
struct irq_chip {
	const char *name; //!< Chip name.
	struct list_head irqs; //!< Assigned irqs.

	void (*chip_handle)(int irq); //!< Chip handler.
	void (*sleep)(struct irq_chip *chip); //!< Put the chip to sleep.
	void (*resume)(struct irq_chip *chip); //!< Resume the chip.
};

CDECL

extern bool in_irq_context(void);
extern int irq_chip_add_irq(struct irq_chip *chip, struct irq_data *irq);
extern int irq_chip_init(struct irq_chip *chip, const char *name);
extern int irq_request(int irq, irq_vector_t vector, unsigned long flags,
			void *irq_data);
extern void irq_handle_fn(void *data);
extern int irq_set_handle(int irq, irq_vector_t vector);
extern struct irq_data *irq_to_data(int irq);

/* IRQ HANDLE FUNCTIONS */
extern void irq_handle(int irq);

/* IRQ CHIP FUNCTIONS */

/**
 * @brief Get the architecture specific IRQ chip.
 * @return The arch IRQ chip.
 * @ingroup archAPI
 */
extern struct irq_chip *arch_get_irq_chip(void);

/**
 * @brief Set the sleep function for an IRQ chip.
 * @param chip to set sleep for.
 * @param sleep Function pointer.
 */
static inline void irq_chip_set_sleep(struct irq_chip *chip,
				      void (*sleep)(struct irq_chip *))
{
	chip->sleep = sleep;
}

/**
 * @brief Set the resume function for an IRQ chip.
 * @param chip to set resume for.
 * @param resume Function pointer.
 */
static inline void irq_chip_set_resume(struct irq_chip *chip,
				      void (*resume)(struct irq_chip *))
{
	chip->resume = resume;
}

/**
 * @brief Store the IRQ flags.
 * @param flags Variable to store the flags into.
 */
static inline void irq_store_flags(unsigned long *flags)
{
	*flags = arch_irq_get_flags();
}

CDECL_END
#endif

/** @} */
