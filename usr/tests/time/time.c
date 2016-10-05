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

#include <uapi/etaos/test.h>
#include <asm/pgm.h>

int main(void)
{
	struct tm *tm;
	time_t now;
	char buf[] = "1234567890";
	int i = 0;

	printf_P(PSTR("Application started!\n"));

	now = (time_t)atol(buf);
	stime(now);

	for(; i < 5; i++) {
		now = time(NULL);
		tm = localtime(&now);
		printf_P(PSTR("Date: %i-%i-%i, %i:%i:%i\n"),
					tm->tm_mday,
					tm->tm_mon,
					tm->tm_year + 1900,
					tm->tm_hour,
					tm->tm_min,
					tm->tm_sec);
		delay(1000);
	}

	printf(CALYPSO_EXIT);
	return 0;
}

