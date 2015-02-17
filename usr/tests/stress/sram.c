/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/delay.h>
#include <etaos/sram.h>

int sram_stress_write_byte(uint16_t addr, uint8_t byte)
{
	int fd, rc;
	unsigned long _addr = addr;
	struct vfile * stream; 

	fd = open("23K256", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		ioctl(stream, SRAM_RESET_WR_IDX, &_addr);
		rc = putc(byte, stream);
		close(fd);
	} else {
		rc = -1;
	}

	/* needs a 10ms delay before the next write can occur */
	return rc;
}

int sram_stress_read_byte(uint16_t addr, uint8_t *store)
{
	int rc, fd;
	unsigned long _addr = addr;
	struct vfile * stream;

	fd = open("23K256", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		ioctl(stream, SRAM_RESET_RD_IDX, &_addr);
		rc = getc(stream);
		*store = (uint8_t)rc;
		close(fd);
	} else {
		*store = -1;
		rc = -1;
	}

	return rc;
}
