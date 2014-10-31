/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/eeprom.h>

#include <etaos/eeprom/24c02.h>

int main(void)
{
	unsigned char readback;
	int rc;
	
	eeprom_init_24c02();
	rc = eeprom_24c02_write_byte(100, 0xAC);
	printf("Write error: %i\n", rc);

	sleep(200);

	while(true) {
		rc = eeprom_24c02_read_byte(100, &readback);
		if(readback == 0xAC)
			printf("[OK] readback: %u\n", readback);
		else
			printf("[ERR] readback: %u\n", readback);
		printf("Read error: %i\n", rc);

		sleep(500);
	}
	return 0;
}

