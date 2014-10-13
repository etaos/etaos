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

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;

THREAD(test_th_handle, arg)
{
	while(true) {
		printf("test_thread\n");
		sleep(1000);
	}
}

int main(void)
{
	printf("Application started (M:%u)!\n", mm_heap_available());
	test_t = thread_create("tst", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_thread_stack, 80);

	printf("PROG EXIT\n");
	while(true) {
		printf("maint\n");
		sleep(500);
	}
	return 0;
}

