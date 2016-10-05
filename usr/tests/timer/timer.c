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

#include <uapi/etaos/test.h>
#include <asm/pgm.h>

static struct timer *tm1;
static char *tm1_name = "tm1";
volatile unsigned long timer_cnt = 0;

static void print_timer(struct timer *timer, void *data)
{
	printf("%lu\n", timer_cnt);
	timer_cnt++;
}

static struct timer *tmtest_init_timers(void)
{
	struct clocksource *cs = sys_clk;

	printf("Adding 1 timer: ");
	tm1 = timer_create_timer(cs, 500, &print_timer, 
			tm1_name, 0);
	
	if(tm1)
		printf("[OK]\n");

	return tm1;
}

int main(void)
{
	unsigned int diff;
	struct clocksource *cs;
	struct timer *timer1;

	printf_P(PSTR("Application started!\n"));

	cs = sys_clk;
	timer1 = tmtest_init_timers();
	diff = clocksource_update(cs);

	while(timer_cnt < 5) {
		timer_process_clock(cs, diff);
		diff = clocksource_update(cs);
	}

	timer_stop_timer(timer1);
	printf(CALYPSO_EXIT);
	return 0;
}

