/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdlib.h>
#include <etaos/stdio.h>
#include <etaos/mutex.h>
#include <etaos/bitops.h>
#include <etaos/mem.h>
#include <etaos/list.h>
#include <etaos/irq.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/platform.h>

#include <asm/io.h>
#include <asm/pgm.h>
#include <asm/time.h>
#include <asm/simulavr.h>

static uint8_t blink_stack[CONFIG_STACK_SIZE];

THREAD(blink_thread, arg)
{
	int value = 1;

	pgpio_pin_request(13);
	pgpio_direction_output(13, true);
	pgpio_pin_release(13);

	while(true) {
		pgpio_pin_request(13);
		pgpio_write_pin(13, value);
		pgpio_pin_release(13);

		value = !value;
		printf_P(PSTR("Value: %i\n"), value);
		sleep(500);
	}
}

int main(void)
{
	struct tm *tm;
	time_t now;
	char buf[16];

	printf("Application started! (%u)\n", mm_heap_available());

	thread_create("blink-th", &blink_thread, NULL,
			CONFIG_STACK_SIZE, blink_stack, 150);
	read(0, buf, 10);
	buf[10] = 0;
	now = (time_t)atol(buf);
	stime(now);

	while(true) {
		now = time(NULL);
		tm = localtime(&now);
		printf_P(PSTR("Date: %i-%i-%i, %i:%i\n"),
					tm->tm_mday,
					tm->tm_mon,
					tm->tm_year + 1900,
					tm->tm_hour,
					tm->tm_min);
		printf_P(PSTR("Memory available: %u\n"), mm_heap_available());
		sleep(1000);
	}

	return 0;
}

