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

static DEFINE_THREAD_QUEUE(test_queue);

THREAD(test_th_handle, arg)
{
	while(true) {
		printf("Hai 1\n");
		event_wait(&test_queue, EVENT_WAIT_INFINITE);
		sleep(100);
	}
}

THREAD(test_th_handle2, arg)
{
	while(true) {
		printf("Hai 2\n");
		event_wait(&test_queue, EVENT_WAIT_INFINITE);
		sleep(100);
	}
}


static volatile unsigned long preempt_counter;
THREAD(preempt_thread, arg)
{
	while(true) {
		printf("Hai 3\n");
		event_wait(&test_queue, EVENT_WAIT_INFINITE);
		sleep(100);
	}
}


int main(void)
{
	printf_P(PSTR("Application started (m: %u)\n"), mm_heap_available());

	thread_create("test-1", &test_th_handle, NULL, NULL);
	thread_create("test-2", &test_th_handle2, NULL, NULL);
	thread_create("preempt", &preempt_thread, NULL, NULL);

	sleep(10);

	while(true) {
		printf("Hai main\n");
		event_notify_broadcast(&test_queue);
		sleep(1000);
	}

	return 0;
}
