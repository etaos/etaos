/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/evm.h>
#include <etaos/mem.h>
#include <etaos/mutex.h>
#include <etaos/time.h>

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;

THREAD(test_th_handle, arg)
{
	nice(150);
	while(true) {
		printf("test_thread\n");
		wait();
	}
}

int main(void)
{
	printf("Application started (M:%u)!\n", mm_heap_available());
	test_t = thread_create( "tst", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_thread_stack, 80);
	
	while(true) {
		printf("maint mem: %u\n", mm_heap_available());
		signal(test_t);
		sleep(500);
	}
	return 0;
}

