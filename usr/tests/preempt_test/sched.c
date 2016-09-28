/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/delay.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/mem.h>

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;

THREAD(test_th_handle, arg)
{
	volatile int *trigger = (int*)arg;

	while(true) {
		if(*trigger) {
			*trigger = false;
		}
	}
}

int main(void)
{
	int trigger = true;

	printf("Application started (M:%u)!\n", mm_heap_available());
	test_t = thread_create( "tst", &test_th_handle, (void*)&trigger,
			CONFIG_STACK_SIZE, test_thread_stack, 120);

	while(true) {
		trigger = true;
		printf("main_thread (%u)\n", trigger);
		sleep(500);
	}

	return 0;
}

