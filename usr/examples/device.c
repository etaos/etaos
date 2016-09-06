/*
 * ETA/OS example application.
 *
 * Author: Michel Megens
 * Date: 06 - 09 - 2016
 */

/**
 * @addtogroup examples
 * @{
 * @tableofcontents
 * @par Talking to device drivers
 *
 * Example segment which displays how to comunicate with device drivers. This
 * example uses the EEPROM 24C02 device driver.
 *
@code{.c}
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
	return -EOK;
}
@endcode
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

