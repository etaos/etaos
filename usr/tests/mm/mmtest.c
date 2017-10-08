/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/unistd.h>
#include <etaos/device.h>
#include <etaos/eeprom.h>
#include <etaos/string.h>

#include <uapi/etaos/test.h>

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

static void mem_test(void)
{
	void *tmp;

	printf("Initial heap: %u\n", mm_heap_available());

	tmp = kmalloc(100);
	printf("Heap after 100-byte alloc: %u\n", mm_heap_available());

	kfree(tmp);
	printf("Heap after 100-byte free: %u\n", mm_heap_available());
}

int main(void)
{
	printf("Application started!\n");

	eeprom_write(EE_STRING);
	mem_test();

	if(eeprom_check(EE_STRING))
		printf_P(PSTR("[ERROR] native eeprom failure!\n"));

	printf(CALYPSO_EXIT);
	return 0;
}
