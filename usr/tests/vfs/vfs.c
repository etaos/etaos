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

#include <asm/pgm.h>
#include <uapi/etaos/test.h>

extern struct dirent vfs_root;

int main(void)
{
	struct dirent *dir;

	printf_P(PSTR("Application started!\n"));

	mkdir("/dev");
	mkdir("/dev/tty");
	mkdir("/dev/block");
	mkdir("/sys");

	dir = dirent_find(&vfs_root, "/dev/tty");
	printf("Found directory: %s\n", dir->name);

	for(;;) {
		printf("Memory available: %u\n", mm_heap_available());
		delay(1000);
	}

	printf(CALYPSO_EXIT);
	return 0;
}

