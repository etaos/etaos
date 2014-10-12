/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/mem.h>
#include <etaos/spinlock.h>
#include <etaos/time.h>

static void thandle(struct timer *tp, void *arg)
{
	printf("hai\n");
}

int main(void)
{
	struct rq *rq = sched_get_cpu_rq();

	printf("Application started (M:%u)!\n", mm_heap_available());
	tm_create_timer(rq->source, 500, &thandle, NULL, 0);

	printf("PROG EXIT\n");
	while(true) {
		printf("maint\n");
		sleep(500);
	}
	return 0;
}

