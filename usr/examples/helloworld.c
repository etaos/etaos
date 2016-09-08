/*
 * ETA/OS example application.
 *
 * Author: Michel Megens
 * Date: 06 - 09 - 2016
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/thread.h>
#include <etaos/stdio.h>

int main(void)
{
	printf("Hello, world!\n");
	for(;;);
	return -EOK;
}

