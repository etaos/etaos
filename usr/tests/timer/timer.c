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
#include <etaos/timer.h>
#include <etaos/clocksource.h>
#include <etaos/tick.h>

#include <asm/io.h>

static struct timer *tm1;
static char *tm1_name = "tm1";
unsigned long timer_cnt = 0;

static void print_timer(struct timer *timer, void *data)
{
	printf("%lu\n", timer_cnt);
	timer_cnt++;
}

static void tmtest_init_timers(void)
{
	struct clocksource *cs = sys_clk;

	printf("Adding 1 timer: ");
	tm1 = timer_create_timer(cs, 500, &print_timer, 
			tm1_name, 0);
	
	if(tm1)
		printf("[OK]\n");
}

int main(void)
{
	unsigned int diff;
	struct clocksource *cs;

	printf("Application started! (%u)\n", mm_heap_available());

	cs = sys_clk;
	tmtest_init_timers();

	diff = clocksource_update(cs);
	while(true) {
		timer_process_clock(cs, diff);
		diff = clocksource_update(cs);
	}

#ifdef CONFIG_SIMUL_AVR
	simul_avr_exit(0);
#endif
	return 0;
}

