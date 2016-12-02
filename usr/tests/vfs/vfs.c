/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/vfs.h>
#include <etaos/delay.h>
#include <etaos/device.h>
#include <etaos/error.h>
#include <etaos/devfs.h>

#include <asm/pgm.h>
#include <uapi/etaos/test.h>

extern struct dirent vfs_root;

static int dummy_write(struct vfile * stream, const void *buf, size_t len)
{
	printf("DummyDev write!\n");
	return -EOK;
}

static int dummy_read(struct vfile * stream, void *buf, size_t len)
{
	printf("DummyDev read!\n");
	return -EOK;
}

static int dummy_open(struct vfile *file)
{
	printf("DummyDev open!\n");
	return -EOK;
}

static int dummy_close(struct vfile *file)
{
	printf("DummyDev close!\n");
	return -EOK;
}

static struct device dummy_dev = {
	.name = "dummy",
};

static struct dev_file_ops dummy_fops = {
	.write = &dummy_write,
	.read = &dummy_read,
	.open = &dummy_open,
	.close = &dummy_close,
};

int main(void)
{
	int fd;

	printf_P(PSTR("Application started! (%u)\n"), mm_heap_available());

	device_initialize(&dummy_dev, &dummy_fops);

	fd = vfs_open("/dev/dummy", _FDEV_SETUP_RW);

	if(fd < 0) {
		fprintf_P(stderr, PSTR("[ERR]: Couldn't open dummy device"));
	} else {
		write(fd, NULL, 0);
		close(fd);
	}

	for(;;) {
		printf("Memory available: %u\n", mm_heap_available());
		delay(1000);
	}

	printf(CALYPSO_EXIT);
	return 0;
}
