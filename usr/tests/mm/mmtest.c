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

static DEFINE_MUTEX(mainlock);
static unsigned long flags = 196608UL;
//static atomic_t atom_var = ATOMIC_INIT(0);

static void lock_test(void)
{
	mutex_lock(&mainlock);
	printf("First lock test: [OK]\n");
	mutex_unlock(&mainlock);
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
	kfree(tmp);
	mm_heap_available();
}

int main(void)
{
	irq_enable();
	printf("Application started!\n");
	lock_test();
	bitops_test();
	mem_test();

#ifdef CONFIG_SIMUL_AVR
	simul_avr_exit(0);
#endif
	return 0;
}

