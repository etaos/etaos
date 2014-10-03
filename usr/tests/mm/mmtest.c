/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mutex.h>
#include <etaos/bitops.h>
#include <etaos/mem.h>
#include <etaos/list.h>
#include <etaos/irq.h>

#include <asm/io.h>
#include <asm/simulavr.h>

static mutex_t mainlock;
static unsigned long flags = 196608UL;
//static atomic_t atom_var = ATOMIC_INIT(0);

static void lock_test(void)
{
	unsigned long flags;

	mainlock.lock = 0;
	mutex_lock_irqsave(&mainlock, flags);
	printf("First lock test: [OK]\n");
	mutex_unlock_irqrestore(&mainlock, flags);
	mutex_lock(&mainlock);
	printf("Last lock test: [OK]\n");
	mutex_unlock(&mainlock);
}

static void bitops_test(void)
{
	if(test_and_clear_bit(16, &flags))
		printf("Flags: %u\n", *(((unsigned char*)&flags)+2));
	set_bit(16, &flags);
	printf("Flags: %u\n", *(((unsigned char*)&flags)+2));
}

static void mem_test(void)
{
	void *tmp;
	mm_heap_available();
	tmp = kmalloc(100);
	mm_heap_available();
	kfree(tmp);
	mm_heap_available();
}

int main(void)
{
//	unsigned long flags;
//	unsigned char irq1, irq2;
	irq_enable();
	printf("Application started!\n");
	lock_test();
	bitops_test();
	mem_test();

//	atomic_inc(&atom_var);
//	irq_save_and_disable(&flags);
//	irq1 = SREG;
//	irq_restore(&flags);
//	irq2 = SREG;

//	printf("Exit %i - %u:%u\n", atomic_get(&atom_var), irq1, irq2);
#ifdef CONFIG_SIMUL_AVR
	simul_avr_exit();
#endif
	return 0;
}

