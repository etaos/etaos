/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/python.h>

#include <uapi/etaos/test.h>

int main(void)
{
	python_init();
	python_start("main");

	while(true);
	return 0;
}

