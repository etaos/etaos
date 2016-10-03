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
#include <etaos/vfs.h>

int sram_stress_write_byte(uint16_t addr, uint8_t byte)
{
	int fd, rc;
	struct vfile * stream; 

	fd = open("23K256", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		lseek(stream, addr, SEEK_SET);
		rc = putc(byte, stream);
		close(fd);
	} else {
		rc = -1;
	}

	return rc;
}

int sram_stress_read_byte(uint16_t addr, uint8_t *store)
{
	int rc, fd;
	struct vfile * stream;

	fd = open("23K256", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		lseek(stream, addr, SEEK_SET);
		rc = getc(stream);
		*store = (uint8_t)rc;
		close(fd);
	} else {
		*store = -1;
		rc = -1;
	}

	return rc;
}

int sram_stress_read(uint16_t addr, void *buff, size_t len)
{
	int rc, fd;
	struct vfile *stream;

	fd = open("23K256", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		lseek(stream, addr, SEEK_SET);
		rc = read(fd, buff, len);
		close(fd);
	} else {
		rc = -1;
	}

	return rc;
}

int sram_stress_write(uint16_t addr, const void *buff, size_t len)
{
	int fd, rc;
	struct vfile *stream;

	fd = open("23K256", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		lseek(stream, addr, SEEK_SET);
		rc = write(fd, buff, len);
		close(fd);
	} else {
		rc = -1;
	}

	return rc;
}

