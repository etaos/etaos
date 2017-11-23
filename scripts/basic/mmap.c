/*
 * Fixdep mmap wrapper for windows.
 */

//#if defined(__MINGW32__) || defined(__MINGW64__)
#define __WINDOWS__
//#endif
 
#ifdef __WINDOWS__
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "mman.h"

#define UNUSED __attribute__ ((__unused__))

/* Workaround specifically for fixdep */
#define PROT_READ 0
#define MAP_PRIVATE 0

void *mmap(void *start UNUSED, size_t size, int prot UNUSED,
	   int flags UNUSED, int fd, off_t offset UNUSED)
{
	void *p;
	void *curP;
	ssize_t readB;

	p = malloc(size);
	if (!p)
		return (void*)((long)-1);

	curP = p;

	while (size > 0)
	{
		readB = read(fd, curP, size);

		if (readB == 0)
		{
			/* EOF reached */
			break;
		}
		else if (readB < 0)
		{
			perror("fixdep: read config");
			free(p);
			return (void*)((long)-1);
		}

		size -= readB;
		curP += readB;
	}

	return p;
}
void munmap(void *p, size_t size UNUSED)
{
	free(p);
}
#endif
