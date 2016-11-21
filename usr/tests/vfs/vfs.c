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

struct dirent vfs_root = {
	.name      = "/",
	.entry     = STATIC_INIT_LIST_HEAD(vfs_root.entry),
	.children  = STATIC_INIT_LIST_HEAD(vfs_root.children),
	.file_head = NULL,
	.fs        = NULL,
};

int main(void)
{
	struct dirent *dir;

	printf_P(PSTR("Application started!\n"));

	dir = dirent_create("dev");
	dirent_add_child(&vfs_root, dir);
	dirent_add_child(dir, dirent_create("tty"));
	dirent_add_child(dir, dirent_create("block"));
	dirent_add_child(&vfs_root, dirent_create("sys"));
	dirent_add_child(&vfs_root, dirent_create("var"));

	dir = dirent_find(&vfs_root, "/dev");
	printf("Found directory: %s\n", dir->name);

	for(;;) {
		printf("Memory available: %u\n", mm_heap_available());
		delay(1000);
	}

	printf(CALYPSO_EXIT);
	return 0;
}

