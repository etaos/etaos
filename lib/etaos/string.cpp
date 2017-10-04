/*
 *  ETA/OS - ETA/OS STL String
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
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
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/stdio.h>

#include <etaos/stl/string.h>

String::String(const char *buf)
{
	this->init();
	this->copy(buf);
}

String::String(const String &str)
{
	this->init();
	*this = str;
}

String::String(char c)
{
	char buf[2];

	this->init();
	buf[0] = c;
	buf[1] = '\0';
	*this = buf;
}

#define BUF_SIZE(__c__) (8*sizeof(__c__))
String::String(unsigned char c)
{
	char buf[1 + BUF_SIZE(c)];

	init();
	snprintf(buf, BUF_SIZE(c), "%u", c);
	*this = buf;
}

String::String(int x)
{
	char buf[2 + BUF_SIZE(x)];

	init();
	snprintf(buf, BUF_SIZE(x), "%i", x);
	*this = buf;
}

String::String(unsigned int x)
{
	char buf[1 + BUF_SIZE(x)];

	init();
	snprintf(buf, BUF_SIZE(x), "%u", x);
	*this = buf;
}

String::String(long x)
{
	char buf[2 + BUF_SIZE(x)];

	init();
	snprintf(buf, BUF_SIZE(x), "%li", x);
	*this = buf;
}

String::String(unsigned long x)
{
	char buf[1 + BUF_SIZE(x)];

	init();
	snprintf(buf, BUF_SIZE(x), "%lu", x);
	*this = buf;
}

String::String(float f)
{
	char buf[4 + BUF_SIZE(f)];

	init();
	snprintf(buf, BUF_SIZE(f), "%f", f);
	*this = buf;
}

String::~String()
{
	kfree(this->buffer);
}

void String::init()
{
	this->buffer = NULL;
}

void String::copy(const char *str)
{
	char *data;

	data = (char*)kzalloc(strlen(str) + 1);
	strcpy(data, str);

	if(this->buffer)
		kfree(this->buffer);

	this->buffer = data;
}

String &String::operator=(const String &other)
{
	this->copy(other.c_str());
	return *this;
}

String &String::operator=(const char *str)
{
	if(str)
		this->copy(str);

	return *this;
}

char &String::front()
{
	return (*this)[0];
}

const char &String::front() const
{
	return (*this)[0];
}

char &String::back()
{
	return (*this)[this->length() - 1];
}

const char &String::back() const
{
	return (*this)[this->length() - 1];
}

/* String concat */
void String::concat(const char *str)
{
	char *buf;
	size_t len;

	len = strlen(str);
	buf = (char*)kzalloc(len + this->length() + 1);
	strcpy(buf, this->buffer);
	strcpy(buf+this->length(), str);

	if(this->buffer)
		kfree(this->buffer);

	//printf("%s\n", buf);
	this->buffer = buf;
}

void String::concat(unsigned char c)
{
	char buf[1 + BUF_SIZE(c)];

	snprintf(buf, BUF_SIZE(c), "%u", c);
	this->concat(buf);
}

void String::concat(int x)
{
	char buf[2 + BUF_SIZE(x)];

	snprintf(buf, BUF_SIZE(x), "%i", x);
	this->concat(buf);
}

void String::concat(unsigned int x)
{
	char buf[1 + BUF_SIZE(x)];

	snprintf(buf, BUF_SIZE(x), "%u", x);
	this->concat(buf);
}

void String::concat(long x)
{
	char buf[2 + BUF_SIZE(x)];

	snprintf(buf, BUF_SIZE(x), "%li", x);
	this->concat(buf);
}

void String::concat(unsigned long x)
{
	char buf[1 + BUF_SIZE(x)];

	snprintf(buf, BUF_SIZE(x), "%lu", x);
	this->concat(buf);
}

void String::concat(float f)
{
	char buf[4 + BUF_SIZE(f)];

	snprintf(buf, BUF_SIZE(f), "%f", f);
	this->concat(buf);
}

bool String::equals(const String &s) const
{
	return equals(s.c_str());
}

bool String::equals(const char *s) const
{
	return strcmp(this->buffer, s) == 0;
}

int String::compareTo(const String &s) const
{
	return strcmp(this->buffer, s.c_str());
}

const char *String::c_str() const
{
	return this->buffer;
}

static char null_terminator = '\0';
const char &String::operator[](size_t index) const
{
	if(index < this->length()) {
		return this->buffer[index];
	}

	return null_terminator;
}

char &String::operator[](size_t index)
{
	if(index < this->length()) {
		return this->buffer[index];
	}

	return null_terminator;
}
