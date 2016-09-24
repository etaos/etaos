/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/hrtimer.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/mem.h>

#include <asm/pgm.h>

static void hrtimer_handle_func(struct hrtimer *hrt, void *arg)
{
	printf("hai\n");
}

int main(void)
{
	struct hrtimer *timer;
	struct hrtimer_source *src;
	int i;

	printf("Application started (M:%u)!\n", mm_heap_available());

	hrtimer_create(hr_sys_clk, 2000000000, hrtimer_handle_func,
			NULL, 0UL);
	hrtimer_create(hr_sys_clk, 500000000, hrtimer_handle_func,
			NULL, 0UL);

	src = container_of(hr_sys_clk, struct hrtimer_source, base);
	timer = src->timers;

	for(i = 0; timer; i++) {
		printf_P(PSTR("Timer %i has %u ticks left.\n"),
				i, timer->ticks);
		timer = timer->next;
	}

	while(true) {
		sleep(1000);
	}

	return 0;
}

