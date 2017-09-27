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
#include <etaos/device.h>
#include <etaos/unistd.h>
#include <etaos/string.h>

#include <uapi/etaos/test.h>

#include <asm/pgm.h>

static struct thread *test_t;
static volatile int counter;

#define EEPROM_ADDR 0x20
#define EE_STRING "test"
#define EE_STRING_LEN 4

static void eeprom_write(const char *data)
{
	int fd;

	fd = open("/dev/atmega-eeprom", _FDEV_SETUP_RW);
	if(fd < 0)
		return;

	lseek(filep(fd), EEPROM_ADDR, SEEK_SET);
	write(fd, data, EE_STRING_LEN);
	close(fd);
}

static int eeprom_check(const char *data)
{
	int fd;
	char buffer[EE_STRING_LEN + 1];

	fd = open("/dev/atmega-eeprom", _FDEV_SETUP_RW);

	if(fd < 0)
		return fd;

	lseek(filep(fd), EEPROM_ADDR, SEEK_SET);
	read(fd, buffer, EE_STRING_LEN);
	close(fd);

	buffer[EE_STRING_LEN] = '\0';
	return strcmp(buffer, EE_STRING);
}

THREAD(test_th_handle, arg)
{
	volatile int *trigger = (int*)arg;

	eeprom_write(EE_STRING);

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
	test_t = thread_create( "tst", &test_th_handle, (void*)&trigger, NULL);

	for(; i < 5; i++) {
		trigger = true;
		printf_P(PSTR("Main thread!\n"));
		sleep(500);
	}

	printf_P(PSTR("Preempt test: "));
	if(counter == 5)
		printf_P(PSTR("[OK]"));
	else
		printf_P(PSTR("[ERROR] (counter = %i)"), counter);
	putc('\n', stdout);

	if(eeprom_check(EE_STRING))
		printf_P(PSTR("[ERROR] native eeprom failure!\n"));

	printf(CALYPSO_EXIT);
	return -EOK;
}

