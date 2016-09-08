/*
 * ETA/OS example application.
 *
 * Author: Michel Megens
 * Date: 06 - 09 - 2016
 */

/**
 * @addtogroup examples
 * @{
 *
 * @par Hello World!
 *
 * Basic "Hello, world" example.
 *
@code{.c}
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
@endcode
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

