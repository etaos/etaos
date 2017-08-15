/*
 *  ETA/OS - Python SRAM library
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
 *  Copyright (C) 2017   Dean Hall
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#undef __FILE_ID__
#define __FILE_ID__ 0x1C

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/python.h>
#include <etaos/eeprom.h>
#include <etaos/vfs.h>
#include <etaos/unistd.h>

#define SRAM_NAME_BUFFER_SIZE 32

int pm_sram_write(const char *name, uint16_t addr,
		const void *buff, size_t length)
{
	char namebuff[SRAM_NAME_BUFFER_SIZE];
	int fd, rc;
	FILE *stream;

	snprintf(namebuff, SRAM_NAME_BUFFER_SIZE, "/dev/%s", name);
	fd = open(namebuff, _FDEV_SETUP_RW);

	if(fd < 0)
		return fd;

	stream = filep(fd);
	lseek(stream, addr, SEEK_SET);
	rc = write(fd, buff, length);
	close(fd);

	return rc;
}

int pm_sram_write_float(const char *name, uint16_t addr, float flt)
{
	char namebuff[SRAM_NAME_BUFFER_SIZE];
	int fd, rc;
	FILE *stream;

	snprintf(namebuff, SRAM_NAME_BUFFER_SIZE, "/dev/%s", name);
	fd = open(namebuff, _FDEV_SETUP_RW);

	if(fd < 0)
		return fd;

	stream = filep(fd);
	lseek(stream, addr, SEEK_SET);
	rc = write(fd, &flt, sizeof(float));
	close(fd);

	return rc;
}

float pm_sram_read_float(const char *name, uint16_t addr)
{
	char namebuff[SRAM_NAME_BUFFER_SIZE];
	int fd;
	float flt;
	FILE *stream;

	snprintf(namebuff, SRAM_NAME_BUFFER_SIZE, "/dev/%s", name);
	fd = open(namebuff, _FDEV_SETUP_RW);

	if(fd < 0)
		return fd;

	stream = filep(fd);
	lseek(stream, addr, SEEK_SET);
	read(fd, &flt, sizeof(float));
	close(fd);

	return flt;
}

int pm_sram_read(const char *name, uint16_t addr, void *buff, size_t length)
{
	char namebuff[SRAM_NAME_BUFFER_SIZE];
	int fd, rc;
	FILE *stream;

	snprintf(namebuff, SRAM_NAME_BUFFER_SIZE, "/dev/%s", name);
	fd = open(namebuff, _FDEV_SETUP_RW);

	if(fd < 0)
		return fd;

	stream = filep(fd);
	lseek(stream, addr, SEEK_SET);
	rc = read(fd, buff, length);
	close(fd);

	return rc;
}

