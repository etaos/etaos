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

int ee_stress_write_byte(uint8_t addr, uint8_t byte)
{
	int fd, rc;
	unsigned long _addr = addr;
	FILE stream;

	fd = open("24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		ioctl(stream, EEPROM_RESET_WR_IDX, &_addr);
		rc = putc(byte, stream);
		close(fd);
	} else {
		rc = -1;
	}

	/* needs a 10ms delay before the next write can occur */
	return rc;
}

int ee_stress_read_byte(uint8_t addr, uint8_t *store)
{
	int rc, fd;
	unsigned long _addr = addr;
	FILE stream;

	fd = open("24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		ioctl(stream, EEPROM_RESET_RD_IDX, &_addr);
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
	unsigned long _addr = addr;
	FILE stream;

	fd = open("24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		ioctl(stream, EEPROM_RESET_RD_IDX, &_addr);
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
	unsigned long _addr = addr;
	FILE stream;

	fd = open("24C02", _FDEV_SETUP_RW);

	if(fd >= 0) {
		stream = filep(fd);
		ioctl(stream, EEPROM_RESET_WR_IDX, &_addr);
		write(fd, buff, len);
		close(fd);
	} else {
		rc = -1;
	}

	/* needs a 10ms delay before the next write can occur */
	return rc;
}

