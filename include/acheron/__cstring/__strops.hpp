/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <acheron/__libdef.hpp>

namespace ach
{
	/**
	 *
	 */
	#define __ONES (~0UL / 0xFF)
	#define __HIGHS (__ONES * 0x80)
	#define __HAS_ZERO(x) (((x) - __ONES) & ~(x) & __HIGHS)

	LIBACHERON size_t strlen(const char *str)
	{
		const char* s = str;
		while (reinterpret_cast<uintptr_t>(s) & (sizeof(size_t) - 1))
		{
			/* unaligned */
			if (!*s)
				return s - str;
			s++;
		}

		/* TODO: word-by-word optimization */
		while (*s)
			s++;

		return s - str;
	}

	LIBACHERON size_t strnlen(const char *s, const size_t max)
	{
		const char* es = s;
		size_t count = 0;
		if (!max || !*es)
			return 0;

		while (count < max && *es)
		{
			es++;
			count++;
		}

		return count;
	}

	LIBACHERON char *strcpy(char *dest, const char *src)
	{
		char* ret = dest;
		while ((*dest++ = *src++) != '\0') {}
		return ret;
	}

	LIBACHERON char *strncpy(char *dest, const char *src, size_t n)
	{
		size_t i;
		for (i = 0; i < n && src[i] != '\0'; i++)
			dest[i] = src[i];

		for (; i < n; i++)
			dest[i] = '\0';

		return dest;
	}

	LIBACHERON size_t strlcpy(char *dest, const char *src, size_t size)
	{
		const char* s = src;
		size_t n = size;

		if (n != 0 && --n != 0)
		{
			do
			{
				if ((*dest++ = *s++) == 0)
					break;
			}
			while (--n != 0);
		}

		if (n == 0)
		{
			if (size != 0)
				*dest = '\0';
			while (*s++);
		}

		return s - src - 1;
	}

	LIBACHERON char *strcat(char *dest, const char *src)
	{
		char* tmp = dest;

		while (*tmp)
			tmp++;

		while ((*tmp++ = *src++) != '\0') {}
		return dest;
	}

	LIBACHERON char *strncat(char *dest, const char *src, size_t n)
	{
		char* tmp = dest;
		while (*tmp)
			tmp++;

		while (n-- > 0 && (*tmp = *src) != '\0')
		{
			tmp++;
			src++;
		}

		*tmp = '\0';
		return dest;
	}

	LIBACHERON size_t strlcat(char *dest, const char *src, size_t size)
	{
		const char* s = src;
		char* d = dest;
		size_t n = size;

		while (n-- != 0 && *d != '\0')
			d++;
		const size_t dlen = d - dest;
		n = size - dlen;

		if (n == 0)
			return dlen + strlen(s);

		while (*s != '\0')
		{
			if (n != 1)
			{
				*d++ = *s;
				n--;
			}
			s++;
		}
		*d = '\0';

		return dlen + (s - src);
	}

	LIBACHERON int strcmp(const char *s1, const char *s2)
	{
		while (*s1 && (*s1 == *s2))
		{
			s1++;
			s2++;
		}

		return static_cast<uint8_t>(*s1) - static_cast<uint8_t>(*s2);
	}

	LIBACHERON int strncmp(const char *s1, const char *s2, size_t n)
	{
		if (n == 0)
			return 0;

		if (!s1 || !s2)
			return s1 ? 1 : (s2 ? -1 : 0);

		while (n)
		{
			const unsigned char c1 = *s1++;
			if (const unsigned char c2 = *s2++;
				c1 != c2)
				return c1 - c2;
			if (!c1)
				break;
			n--;
		}

		return 0;
	}

	LIBACHERON int strcasecmp(const char *s1, const char *s2)
	{
		if (!s1 || !s2)
			return s1 ? 1 : (s2 ? -1 : 0);

		uint8_t c1 = {};
		uint8_t c2 = {};
		do
		{
			c1 = *s1++;
			c2 = *s2++;

			c1 += (c1 >= 'A' && c1 <= 'Z') ? 'a' - 'A' : 0;
			c2 += (c2 >= 'A' && c2 <= 'Z') ? 'a' - 'A' : 0;

			if (c1 == '\0')
				return c1 - c2;
		} while (c1 == c2);

		return c1 - c2;
	}

	LIBACHERON int strncasecmp(const char *s1, const char *s2, size_t len)
	{
		unsigned char c1;
		unsigned char c2;

		if (!len)
			return 0;

		do
		{
			c1 = (*s1++ | 0x20);
			c2 = (*s2++ | 0x20);

			if (!--len)
				return c1 - c2;
			if (c1 == '\0')
				return c1 - c2;
		}
		while (c1 == c2);

		return c1 - c2;
	}

	LIBACHERON char *strchr(const char *s, int c)
	{
		for (; *s != static_cast<char>(c); s++)
			if (*s == '\0')
				return nullptr;

		return const_cast<char*>(s);
	}

	LIBACHERON char *strrchr(const char *s, int c)
	{
		const char* last = nullptr;
		do
		{
			if (*s == static_cast<char>(c))
				last = s;
		}
		while (*s++);
		return const_cast<char*>(last);
	}

	LIBACHERON char *strnchr(const char *s, size_t count, int c)
	{
		while (count--)
		{
			if (*s == static_cast<char>(c))
				return const_cast<char*>(s);
			if (*s++ == '\0')
				break;
		}
		return nullptr;
	}

	LIBACHERON char *strnstr(const char *s1, const char *s2, size_t len)
	{
		size_t l2 = strlen(s2);

		if (!l2)
			return const_cast<char*>(s1);

		while (len >= l2)
		{
			auto match = true;
			for (size_t i = 0; i < l2; i++)
			{
				if (s1[i] != s2[i])
				{
					match = false;
					break;
				}
			}

			if (match)
				return const_cast<char*>(s1);

			s1++;
			len--;
		}

		return nullptr;
	}

	LIBACHERON char *strstr(const char *haystack, const char *needle)
	{
		if (!haystack || !needle)
			return nullptr;

		if (*needle == '\0')
			return const_cast<char*>(haystack);

		const char first = *needle;
		while (*haystack)
		{
			if (*haystack == first)
			{
				const char* h = haystack + 1;
				const char* n = needle + 1;
				while (*h && *n && *h == *n)
				{
					h++;
					n++;
				}

				if (*n == '\0')
					return const_cast<char*>(haystack);
			}
			haystack++;
		}
		return nullptr;
	}
}
