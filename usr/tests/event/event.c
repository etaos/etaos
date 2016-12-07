/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/stdlib.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/event.h>
#include <etaos/mem.h>
#include <etaos/vfs.h>
#include <etaos/platform.h>

#include <asm/pgm.h>

#include <uapi/etaos/test.h>

THREAD(test_th_handle1, arg)
{
	sleep(1000);
	printf("Thread 1\n");
	kill();
}

THREAD(test_th_handle2, arg)
{
	sleep(1000);
	printf("Thread 2\n");
	kill();
}


THREAD(test_th_handle3, arg)
{
	sleep(5000);
	printf("Thread 3\n");
	kill();
}


int main(void)
{
	struct thread *tp1, *tp2, *tp3;

	printf_P(PSTR("Application started\n"), mm_heap_available());

	tp1 = thread_create("test-1", &test_th_handle1, NULL, NULL);
	tp2 = thread_create("test-2", &test_th_handle2, NULL, NULL);
	tp3 = thread_create("test-3", &test_th_handle3, NULL, NULL);

	printf("Main thread\n");
	join(tp1);
	join(tp2);
	join(tp3);

	printf(CALYPSO_EXIT);
	return 0;
}
