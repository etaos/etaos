/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/eeprom.h>
#include <etaos/delay.h>

#include <etaos/eeprom/24c02.h>

int main(void)
{
	unsigned char readback;
	int fd;

	fd = open("24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		printf("Writng to eeprom!\n");
		putc(0xAC, filep(fd));
	}
	close(fd);
	
	delay(200);
	while(true) {
		fd = open("24C02", _FDEV_SETUP_RW);
		if(fd >= 0)
			readback = getc(filep(fd));
		else
			readback = (unsigned char)-1;
		close(fd);

		if(readback == 0xAC)
			printf("[OK] readback: %u\n", readback);
		else
			printf("[ERR] readback: %u\n", readback);

		delay(500);
	}
	return 0;
}

