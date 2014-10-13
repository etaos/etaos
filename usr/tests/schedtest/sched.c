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
#include <etaos/spinlock.h>
#include <etaos/time.h>

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;

DEFINE_THREAD_QUEUE(test_queue);

static const char *expi = "expired";
static const char *_signal = "signaled";

THREAD(test_th_handle, arg)
{
	int x;

	while(true) {
		x = evm_wait_next_event_queue(&test_queue, 1000);
		printf("test_thread t:%s\n", x ? expi : _signal);
		sleep(1000);
		evm_signal_event_queue(&test_queue);
	}
}

extern char __heap_start;

int main(void)
{
	char x;
	printf("Application started (M:%u)!\n", mm_heap_available());
	test_t = thread_create( "tst", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_thread_stack, 80);
	
	x = 0;
	while(true) {
		if(x == 3) {
			x = 0;
			evm_signal_event_queue(&test_queue);
		}

		printf("maint mem: %u\n", mm_heap_available());
		x++;
		sleep(500);
	}
	return 0;
}

