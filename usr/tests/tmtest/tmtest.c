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

static struct clocksource cs;
static struct timer *tm1, *tm2, *tm3;

static char *tm1_name = "tm1";
static char *tm2_name = "tm2";
static char *tm3_name = "tm3";

static void tmtest_init_timers(void)
{
	int err;

	err = tm_clock_source_initialise("test-clock", &cs, 1000UL, NULL, NULL);
	printf("Trying to add CS: %i\n", err);
	printf("Adding 3 timers:\n");
	tm1 = tm_create_timer(&cs, 500, NULL, tm1_name, 0);
	tm2 = tm_create_timer(&cs, 200, NULL, tm2_name, 0);
	tm3 = tm_create_timer(&cs, 700, NULL, tm3_name, 0);
	
	if(tm1)
		printf("[OK] Timer 1 added\n");
	if(tm2)
		printf("[OK] Timer 2 added\n");
	if(tm3)
		printf("[OK] Timer 3 added\n");
}

static void tmtest_list_timers(struct clocksource *csource)
{
	struct list_head *carriage;
	struct timer *timer;

	list_for_each(carriage, &csource->timers) {
		timer = list_entry(carriage, struct timer, list);
		printf("Ticks left: %u :: name: ", timer->tleft);
		printf(timer->priv_data);
		printf("\n");
	}
}

int main(void)
{
	irq_enable();
	printf("Application started!\n");
	tmtest_init_timers();
	tmtest_list_timers(&cs);
	printf("Removing timer 2\n");
	tm_stop_timer(tm2);
	tmtest_list_timers(&cs);
#ifdef CONFIG_SIMUL_AVR
	simul_avr_exit(0);
#endif
	return 0;
}

