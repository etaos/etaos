/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>

#include <etaos/stl/string.h>

#include <uapi/etaos/test.h>

int main(void)
{
	String s("Test");
	String s2("Test data 1");

	s += " data ";
	s += (int)1;

	printf("Str: %s\n", s.c_str());
	printf("Equal? %u\n", s == s2);

	printf(CALYPSO_EXIT);
	return -EOK;
}

