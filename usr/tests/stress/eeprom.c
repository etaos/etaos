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
#include <etaos/eeprom.h>
#include <etaos/vfs.h>

int ee_stress_write_byte(uint8_t addr, uint8_t byte)
{
	int fd, rc;
	struct vfile * stream; 

	fd = open("/dev/24C02", _FDEV_SETUP_RW);

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

int ee_stress_read_byte(uint8_t addr, uint8_t *store)
{
	int rc, fd;
	struct vfile * stream;

	fd = open("/dev/24C02", _FDEV_SETUP_RW);

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

int ee_stress_read(uint8_t addr, void *buff, size_t len)
{
	int rc, fd;
	struct vfile *stream;

	fd = open("/dev/24C02", _FDEV_SETUP_RW);

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

int ee_stress_write(uint8_t addr, const void *buff, size_t len)
{
	int fd, rc = -EOK;
	struct vfile *stream;

	fd = open("/dev/24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		lseek(stream, addr, SEEK_SET);
		write(fd, buff, len);
		close(fd);
	} else {
		rc = -1;
	}

	return rc;
}

