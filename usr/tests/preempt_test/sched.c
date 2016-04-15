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
	nice(80);

	while(true) {
		printf("test_thread\n");
		delay(500);
	}
}

int main(void)
{
	const char msg[] = "hello main\n";
	int fd;

	printf("Application started (M:%u)!\n", mm_heap_available());
	test_t = thread_create( "tst", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_thread_stack, 120);

	
	while(true) {
		fd = open("atm-usart", _FDEV_SETUP_RW);
		write(fd, msg, sizeof(msg));
		close(fd);

		printf("main_thread\n");
		delay(500);
	}

	return 0;
}

