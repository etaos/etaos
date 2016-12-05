/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/mem.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/ipm.h>

#include <asm/pgm.h>
#include <uapi/etaos/test.h>

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;
static struct ipm_queue ipm_q;

THREAD(test_th_handle, arg)
{
	int fd;
	struct ipm msg;
	volatile int *idx = arg;

	fd = to_fd(stdout);

	nice(150);

	while(*idx < 2) {
		ipm_get_msg(&ipm_q, &msg);
		ipm_reset_queue(&ipm_q);
		printf_P(PSTR("[test]:\t"));
		write(fd, msg.data, msg.len);
		printf_P(PSTR("[test]:\ttest_thread\n"));
	}

	kill();
}

int main(void)
{
	const char * ip_msg = "IPM message\n";
	int64_t tick_orig;
	int idx = 0;
	thread_attr_t attr;

	printf_P(PSTR("Application started!\n"));
	ipm_queue_init(&ipm_q, 2);

	attr.prio = 130;
	attr.stack = test_thread_stack;
	attr.stack_size = CONFIG_STACK_SIZE;
	test_t = thread_create( "tst", &test_th_handle, &idx, &attr);

	tick_orig = sys_tick;

	for(; idx < 5; idx++) {
		if(time_after(sys_tick, tick_orig + 1000))
			printf_P(PSTR("[main]:\ttm after: 1000\n"));
		ipm_post_msg(&ipm_q, ip_msg, strlen(ip_msg));
		printf_P(PSTR("[main]:\tIPM posted\n"));

		sleep(500);
	}

	printf(CALYPSO_EXIT);
	return -EOK;
}

