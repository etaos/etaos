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
#include <etaos/time.h>

#include <asm/io.h>
#include <asm/simulavr.h>

extern volatile unsigned long test_sys_tick;

static void sysclk_ping(void)
{
	int i = 0;

	while(1) {
		if((test_sys_tick % 500) == 0 && test_sys_tick != 0) {
			i++;
			printf("[SYSCLK] ping!\n");
		}

		if(i == 3)
			break;
	}
}

int main(void)
{
	struct clocksource *src;
	int64_t sys_tick;

	irq_enable();
	printf("Application started!\n");
	sysclk_ping();

	src = tm_get_source_by_name("sys-clk");
	sys_tick = atomic64_get(&src->tc);
	printf("OK: Clock finished: %lli::%lu - prog end\n", 
			sys_tick, test_sys_tick);
#ifdef CONFIG_SIMUL_AVR
	simul_avr_exit();
#endif
	return 0;
}

