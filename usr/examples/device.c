/*
 * ETA/OS example application.
 *
 * Author: Michel Megens
 * Date: 06 - 09 - 2016
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/panic.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/delay.h>
#include <etaos/eeprom.h>
#include <etaos/vfs.h>

#define EEPROM_ADDR 0x20

int main(void)
{
	struct vfile *stream;
	int fd;

	fd = open("24C02", _FDEV_SETUP_RW);
	
	if(fd < 0)
		panic("could not open 24C02 device file\n");

	stream = filep(fd);
	lseek(stream, EEPROM_ADDR, SEEK_SET);
	putc('A', stream);
	close(fd);

	for(;;);
}

/** @} */

