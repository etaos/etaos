/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdlib.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/time.h>
#include <etaos/delay.h>

#include <asm/pgm.h>

int main(void)
{
	struct tm *tm;
	time_t now;
	char buf[] = "1234567890";

	printf_P(PSTR("Application started! (%u)\n"), mm_heap_available());

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
		delay(1000);
	}

	return 0;
}

