/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/delay.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/mem.h>

#include <uapi/etaos/test.h>

#include <asm/pgm.h>

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;
static volatile int counter;

THREAD(test_th_handle, arg)
{
	volatile int *trigger = (int*)arg;

	while(true) {
		if(*trigger) {
			*trigger = false;
			if(counter++ == 5)
				break;
		}
	}

	kill();
}

int main(void)
{
	int trigger = true, i = 0;

	printf_P(PSTR("Application started!\n"));

	counter = 0;
	test_t = thread_create( "tst", &test_th_handle, (void*)&trigger,
			CONFIG_STACK_SIZE, test_thread_stack, 120);

	for(; i < 5; i++) {
		trigger = true;
		printf_P(PSTR("Main thread!\n"));
		sleep(500);
	}

	printf_P(PSTR("Preempt test: "));
	if(counter == 5)
		printf_P(PSTR("[OK]"));
	else
		printf_P(PSTR("[ERROR]"));
	putc('\n', stdout);

	printf(CALYPSO_EXIT);
	return -EOK;
}

