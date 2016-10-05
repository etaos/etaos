/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/vfs.h>
#include <etaos/thread.h>

#include <asm/pgm.h>
#include <uapi/etaos/test.h>

#define EEPROM_WRITE_ADDR 0x20

int main(void)
{
	int fd;
	struct vfile *file;
	unsigned char readback = 0;

	irq_enable();
	printf_P(PSTR("Application started!\n"));

	fd = open("24C02", _FDEV_SETUP_RW);
	if(fd >= 0) {
		file = filep(fd);
		lseek(file, EEPROM_WRITE_ADDR, SEEK_SET);
		putc(0xBA, file);
		
		/*
		 * now get the byte back
		 */
		lseek(file, EEPROM_WRITE_ADDR, SEEK_SET);
		readback = getc(file);
		close(fd);
	}

	if(readback == 0xBA)
		printf("[OK] EEPROM test passed!\n");
	else
		printf("[ERROR] EEPROM test failed!\n");

	printf(CALYPSO_EXIT);
	return 0;
}

