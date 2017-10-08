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
	String s("String");
	String s2("Hey");

	s += " plus ";
	s += (int)5;
	s += " ";
	s += 5.1234f;
	printf("Str: %s\n", s.c_str());
	printf("Equal? %u\n", String("Hey") == s2);

	printf(CALYPSO_EXIT);
	return -EOK;
}

