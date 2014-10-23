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
#include <etaos/gpio.h>
#include <etaos/platform.h>
#include <etaos/ipm.h>

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;
static struct ipm_queue ipm_q;

THREAD(test_th_handle, arg)
{
	int fd;
	struct ipm msg;

	fd = open("atm-usart", _FDEV_SETUP_RW);

	nice(150);

	while(true) {
		ipm_get_msg(&ipm_q, &msg);
		ipm_reset_queue(&ipm_q);
		write(fd, msg.data, msg.len);
		printf("test_thread\n");
	}
}

int main(void)
{
	const char * ip_msg = "IPM message\n";
	bool value = true;

	printf("Application started (M:%u)!\n", mm_heap_available());
	ipm_queue_init(&ipm_q, 2);
	test_t = thread_create( "tst", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_thread_stack, 80);

	pgpio_pin_request(13);
	pgpio_direction_output(13, false);
	pgpio_pin_release(13);

	while(true) {
		ipm_post_msg(&ipm_q, ip_msg, strlen(ip_msg));
		printf("maint mem: %u\n", mm_heap_available());

		pgpio_pin_request(13);
		pgpio_write_pin(13, value);
		pgpio_pin_release(13);
		value = !value;

		sleep(500);
	}
	return 0;
}

