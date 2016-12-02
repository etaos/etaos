/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/vfs.h>
#include <etaos/thread.h>

#include <asm/pgm.h>
#include <uapi/etaos/test.h>

#define SRAM_WRITE_ADDR 0x20

static int test_sram_read(uint16_t addr, void *buff, size_t len)
{
	int rc, fd;
	struct vfile *stream;

	fd = open("/dev/23K256", _FDEV_SETUP_RW);

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

static int test_sram_write(uint16_t addr, const void *buff, size_t len)
{
	int fd, rc;
	struct vfile *stream;

	fd = open("/dev/23K256", _FDEV_SETUP_RW);

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

int main(void)
{
	const char *data = "SRAM test data";
	char *readback;
	int len;

	printf_P(PSTR("Application started!\n"));
	printf_P(PSTR("Write to SRAM: "));

	len = strlen(data);
	readback = kzalloc(len + 1);
	if(test_sram_write(SRAM_WRITE_ADDR, data, len) != -1)
		printf_P(PSTR("[OK]\n"));
	else
		printf_P(PSTR("[FAIL]\n"));

	printf_P(PSTR("Read from SRAM: "));

	if(test_sram_read(SRAM_WRITE_ADDR, readback, len) == -1)
		printf_P(PSTR("[FAIL]\n"));

	if(!strcmp(data, readback))
		printf_P(PSTR("[OK]\n"));
	else
		printf_P(PSTR("[FAIL]\n"));

	kfree(readback);

	printf(CALYPSO_EXIT);
	return 0;
}

