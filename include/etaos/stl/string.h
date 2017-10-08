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

/** @file include/etaos/stl/string.h */
/**
 * @addtogroup stl
 * @{
 */

#ifndef  __STL_STRING_H__
#define __STL_STRING_H__

#include <etaos/kernel.h>
#include <etaos/string.h>

/**
 * @brief STL string class.
 *
 * Strings are objects that represent sequences of characters. The standard
 * string class provides support for such objects with an interface similar to
 * that of a standard container of bytes, but adding features specifically
 * designed to operate with strings of single-byte characters.
 */
class String {
public:
	/**
	 * @brief Create new string.
	 * @param c Character to create a string from.
	 */
	explicit String(char c);
	/**
	 * @brief Create new string.
	 * @param i Integer to create a string from.
	 */
	explicit String(int i);
	/**
	 * @brief Create new string.
	 * @param l Long integer to create a string from.
	 */
	explicit String(long l);
	/**
	 * @brief Create new string.
	 * @param c Character to create a string from.
	 */
	explicit String(unsigned char c);
	/**
	 * @brief Create new string.
	 * @param c Integer to create a string from.
	 */
	explicit String(unsigned int c);
	/**
	 * @brief Create new string.
	 * @param c Long to create a string from.
	 */
	explicit String(unsigned long c);
	/**
	 * @brief Create new string.
	 * @param c Float to create a string from.
	 */
	explicit String(float c);

	/**
	 * @brief Create new string.
	 * @param str String to copy into the new string.
	 */
	String(const char *str = "");
	/**
	 * @brief Create new string.
	 * @param str String to copy into the new string.
	 */
	String(const String &str);

	/**
	 * @brief String destructor
	 */
	virtual ~String();

	/**
	 * @brief String assignment.
	 * @param other String to copy during the assignment.
	 * @return *this
	 */
	String &operator=(const String &other);
	/**
	 * @brief String assignment.
	 * @param other String to copy during the assignment.
	 * @return *this
	 */
	String &operator=(const char *other);

	/**
	 * @brief String append.
	 * @param str String to append.
	 * @return *this
	 */
	String & operator += (const String &str) { concat(str); return *this; }
	/**
	 * @brief String assignment.
	 * @param cstr String to copy during the assignment.
	 * @return *this
	 */
	String & operator += (const char *cstr) { concat(cstr); return *this; }
	/**
	 * @brief String append.
	 * @param c Character to append.
	 * @return *this
	 */
	String & operator += (char c) { concat(c); return *this; }
	/**
	 * @brief String append.
	 * @param num Byte value to append.
	 * @return *this
	 */
	String & operator += (unsigned char num) { concat(num); return *this; }
	/**
	 * @brief String append.
	 * @param num Integer to append.
	 * @return *this
	 */
	String & operator += (int num) { concat(num); return *this; }
	/**
	 * @brief String append.
	 * @param num Integer to append.
	 * @return *this
	 */
	String & operator += (unsigned int num) { concat(num); return *this; }
	/**
	 * @brief String append.
	 * @param num Integer to append.
	 * @return *this
	 */
	String & operator += (long num) { concat(num); return *this; }
	/**
	 * @brief String append.
	 * @param num Integer to append.
	 * @return *this
	 */
	String & operator += (unsigned long num) { concat(num); return *this; }
	/**
	 * @brief String append.
	 * @param num Floating point number to append.
	 * @return *this
	 */
	String & operator += (float num) { concat(num); return *this; }

	/**
	 * @brief Concatinate to \p this string.
	 * @param str String to concatinate to \p this.
	 * @return *this
	 */
	void concat(const char *str);
	/**
	 * @brief Concatinate to \p this string.
	 * @param str String to concatinate to \p this.
	 * @return *this
	 */
	void concat(const String &str);
	/**
	 * @brief Concatinate to \p this string.
	 * @param x Integer to append to \p this string.
	 * @return *this
	 */
	void concat(int x);
	/**
	 * @brief Concatinate to \p this string.
	 * @param x Integer to append to \p this string.
	 * @return *this
	 */
	void concat(char x);
	/**
	 * @brief Concatinate to \p this string.
	 * @param x Character to append to \p this string.
	 * @return *this
	 */
	void concat(long x);
	/**
	 * @brief Concatinate to \p this string.
	 * @param x Integer to append to \p this string.
	 * @return *this
	 */
	void concat(unsigned char x);
	/**
	 * @brief Concatinate to \p this string.
	 * @param x Integer to append to \p this string.
	 * @return *this
	 */
	void concat(unsigned int x);
	/**
	 * @brief Concatinate to \p this string.
	 * @param x Integer to append to \p this string.
	 * @return *this
	 */
	void concat(unsigned long x);
	/**
	 * @brief Concatinate to \p this string.
	 * @param x Floating point number to append to \p this string.
	 * @return *this
	 */
	void concat(float x);

	/**
	 * @brief Get the length of \p this string.
	 * @return The length of \p this string.
	 */
	size_t length() const { return strlen(this->buffer); }
	/**
	 * @brief Check if string is empty.
	 * @return True if string is empty.
	 */
	bool empty() const { return this->length() == 0; }
	/**
	 * @brief Return the last character of \p this string.
	 * @return The last character.
	 */
	char& back();
	/**
	 * @brief Return the last character of \p this string.
	 * @return The last character.
	 */
	const char& back() const;

	/**
	 * @brief Return the first character of this string.
	 * @return The first character.
	 */
	char& front();
	/**
	 * @brief Return the first character of this string.
	 * @return The first character.
	 */
	const char& front() const;

	/**
	 * @brief Check two strings for equality.
	 * @param s String to check \p this against.
	 * @return True if both \p s and \p this are equal.
	 */
	bool equals(const String &s) const;
	/**
	 * @brief Check two strings for equality.
	 * @param str String to check \p this against.
	 * @return True if both \p s and \p this are equal.
	 */
	bool equals(const char *str) const;
	/**
	 * @brief Check two strings.
	 * @param s String to check \p this against.
	 * @return Less than, equal to or greater than 0 depending on whether
	 *         \p s is less than equal to or greater than \p this.
	 */
	int compareTo(const String &s) const;

	/**
	 * @brief Check two strings for equality.
	 * @param other String to check \p this against.
	 * @return True if both \p s and \p this are equal.
	 */
	bool operator==(const String& other) const { return equals(other); }
	/**
	 * @brief Check two strings for equality.
	 * @param other String to check \p this against.
	 * @return True if both \p s and \p this are equal.
	 */
	bool operator==(const char *other) const { return equals(other); }

	/**
	 * @brief Element accessor.
	 * @param pos Index in string.
	 * @return The character found at \p pos.
	 */
	char &operator[](size_t pos);
	/**
	 * @brief Element accessor.
	 * @param pos Index in string.
	 * @return The character found at \p pos.
	 */
	const char &operator[](size_t pos) const;
	/**
	 * @brief Get the C string representation of this string.
	 * @return The C string version of \p this string.
	 */
	const char *c_str() const;

protected:
	void copy(const char *str);

private:
	void init();
	char *buffer;
};

#endif /* end of include guard: __STL_STRING_H__ */

/** @} */
