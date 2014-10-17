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

static DEFINE_THREAD_QUEUE(test_q);
static const char *test_str = "test_str\n";

THREAD(test_th_handle, arg)
{
	int fd;

	fd = open("atm-usart", _FDEV_SETUP_RW);
	if(fd > 0) {
		write(fd, test_str, strlen(test_str));
		close(fd);
	}
	
	nice(150);

	while(true) {

		evm_wait_next_event_queue(&test_q, 1000);
		printf("test_thread\n");
	}
}

int main(void)
{
	printf("Application started (M:%u)!\n", mm_heap_available());
	test_t = thread_create( "tst", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_thread_stack, 80);
	
	while(true) {
		evm_wait_next_event_queue(&test_q, 500);
		printf("maint mem: %u\n", mm_heap_available());
	}
	return 0;
}

