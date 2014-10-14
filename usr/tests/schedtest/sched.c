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

static DEFINE_MUTEX(test_mutex);

THREAD(test_th_handle, arg)
{
	while(true) {
		mutex_lock(&test_mutex);
		printf("test_thread\n");
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
			mutex_unlock(&test_mutex);
		}

		printf("maint mem: %u\n", mm_heap_available());
		x++;
		sleep(500);
	}
	return 0;
}

