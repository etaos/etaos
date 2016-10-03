/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>

static void mem_test(void)
{
	void *tmp;

	printf("Initial heap: %u\n", mm_heap_available());

	tmp = kmalloc(100);
	printf("Heap after 100-byte alloc: %u\n", mm_heap_available());

	kfree(tmp);
	printf("Heap after 100-byte free: %u\n", mm_heap_available());
}

int main(void)
{
	printf("Application started!\n");
	mem_test();

	return 0;
}

