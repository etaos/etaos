/*
 * Windows wrapper for mmap munmap.
 *
 * Author: Michel Megens <dev@bietje.net>
 * License: This file is part of the public domain.
 */

#ifndef __MMAP_WRAPPER_H__
#define __MMAP_WRAPPER_H__

#define PROT_READ 0
#define MAP_PRIVATE 0
extern void munmap(void *p, size_t size);
extern void *mmap(void *start, size_t size, int prot, int flags, int fd, off_t offset);

#endif
