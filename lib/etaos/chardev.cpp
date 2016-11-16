/*
 *  ETA/OS - Platform implementation
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
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

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/vfs.h>

#include <etaos/stl/chardev.h>

CharacterDevice::CharacterDevice(const char *file)
{
	this->fd = open(file, _FDEV_SETUP_RW);

	if(this->fd < 0)
		return;

	this->devfile = filep(this->fd);
}

CharacterDevice::~CharacterDevice(void)
{
	if(this->fd > 0);
		close(this->fd);
}

bool CharacterDevice::check(void)
{
	if(this->fd < 0)
		return false;

	return true;
}

int CharacterDevice::read(void *buff, size_t& len)
{
	if(!this->check())
		return -EINVAL;

	return ::read(this->fd, buff, len);
}

int CharacterDevice::write(const void *buff, size_t& len)
{
	if(!this->check())
		return -EINVAL;

	return ::write(this->fd, buff, len);
}

int CharacterDevice::putc(int& c)
{
	if(!this->check())
		return -EINVAL;

	return ::putc(c, this->devfile);
}

int CharacterDevice::getc(void)
{
	if(!this->check())
		return -EINVAL;

	return ::getc(this->devfile);
}

