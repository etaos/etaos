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
#include <asm/time.h>
#include <asm/simulavr.h>

static struct timer *tm1;
static char *tm1_name = "tm1";

static void print_timer(struct timer *timer, void *data)
{
	char *name = data;
	printf("Exec: %s\n", name);
}

static void tmtest_init_timers(void)
{
	struct clocksource *cs = avr_get_sys_clk();

	printf("Adding 3 timers:\n");
	tm1 = tm_create_timer(cs, 500, &print_timer, 
			tm1_name, 0);
	
	if(tm1)
		printf("[OK] Timer 1 added\n");
}

int main(void)
{
	int64_t diff;
	struct clocksource *cs;

	printf("Application started!\n");
	
	cs = avr_get_sys_clk();
	diff = tm_update_source(cs);
	tmtest_init_timers();

	while(true) {
		tm_process_clock(cs, diff);
		diff = tm_update_source(cs);
	}

#ifdef CONFIG_SIMUL_AVR
	simul_avr_exit(0);
#endif
	return 0;
}

