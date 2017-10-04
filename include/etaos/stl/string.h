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

#ifndef  __STL_STRING_H__
#define __STL_STRING_H__

#include <etaos/kernel.h>
#include <etaos/string.h>

class String {
public:
	explicit String(char c);
	explicit String(int i);
	explicit String(long l);
	explicit String(unsigned char c);
	explicit String(unsigned int c);
	explicit String(unsigned long c);
	explicit String(float c);

	String(const char *str = "");
	String(const String &str);
	virtual ~String();

	String &operator=(const String &other);
	String &operator=(const char *);

	String & operator += (const String &str) { concat(str); return *this; }
	String & operator += (const char *cstr) { concat(cstr); return *this; }
	String & operator += (char c) { concat(c); return *this; }
	String & operator += (unsigned char num) { concat(num); return *this; }
	String & operator += (int num) { concat(num); return *this; }
	String & operator += (unsigned int num) { concat(num); return *this; }
	String & operator += (long num) { concat(num); return *this; }
	String & operator += (unsigned long num) { concat(num); return *this; }
	String & operator += (float num) { concat(num); return *this; }

	void concat(const char *str);
	void concat(const String &str);
	void concat(int x);
	void concat(char x);
	void concat(long x);
	void concat(unsigned char x);
	void concat(unsigned int x);
	void concat(unsigned long x);
	void concat(float x);

	size_t length() const { return strlen(this->buffer); }
	bool empty() const { return this->length() == 0; }
	char& back();
	const char& back() const;

	char& front();
	const char& front() const;

	bool equals(const String &s) const;
	bool equals(const char *str) const;
	int compareTo(const String &s) const;

	bool operator==(const String& other) const { return equals(other); }
	bool operator==(const char *other) const { return equals(other); }


	char &operator[](size_t pos);
	const char &operator[](size_t pos) const;
	const char *c_str() const;

protected:
	void copy(const char *str);

private:
	void init();
	char *buffer;
};

#endif /* end of include guard: __STL_STRING_H__ */
