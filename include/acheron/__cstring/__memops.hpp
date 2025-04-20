/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <acheron/__libdef.hpp>

namespace ach
{
	/**
	 * @brief Compare memory areas
	 *
	 * @param cs Pointer to first memory area
	 * @param ct Pointer to second memory area
	 * @param count Number of bytes to compare
	 * @return int Zero if memory regions are identical, otherwise the difference
	 *         between the first pair of differing bytes (as unsigned values)
	 *
	 * @note Uses word-aligned comparisons for sizes >= 8 bytes
	 * @note The comparison is performed in a left-to-right order
	 * @note Implementation-defined: Returns positive if first differing byte in cs > ct,
	 */
	LIBACHERON int memcmp(const void *cs, const void *ct, size_t count)
	{
		const auto *s1 = static_cast<const uint8_t *>(cs);
		const auto *s2 = static_cast<const uint8_t *>(ct);
		if (count < 8)
		{
			while (count--)
			{
				if (*s1 != *s2)
					return *s1 - *s2;
				s1++;
				s2++;
			}
			return 0;
		}

		/* align to prevent UB */
		size_t align = -reinterpret_cast<uintptr_t>(s1) & (sizeof(size_t) - 1);
		count -= align;
		while (align--)
		{
			if (*s1 != *s2)
				return *s1 - *s2;
			s1++;
			s2++;
		}

		const auto *w1 = reinterpret_cast<const size_t *>(s1);
		const auto *w2 = reinterpret_cast<const size_t *>(s2);
		while (count >= sizeof(size_t))
		{
			if (*w1 != *w2)
			{
				s1 = reinterpret_cast<const uint8_t *>(w1);
				s2 = reinterpret_cast<const uint8_t *>(w2);
				for (size_t i = 0; i < sizeof(size_t); i++)
				{
					if (s1[i] != s2[i])
						return s1[i] - s2[i];
				}
			}
			w1++;
			w2++;
			count -= sizeof(size_t);
		}

		s1 = reinterpret_cast<const uint8_t *>(w1);
		s2 = reinterpret_cast<const uint8_t *>(w2);
		while (count--)
		{
			if (*s1 != *s2)
				return *s1 - *s2;
			s1++;
			s2++;
		}

		return 0;
	}

	/**
	 * @brief Copy memory area
	 *
	 * @param dest Pointer to destination memory area
	 * @param src Pointer to source memory area
	 * @param count Number of bytes to copy
	 * @return void* The original value of dest
	 *
	 * @note Uses word-aligned copy for sizes >= 8 bytes when pointers are properly aligned
	 * @note Undefined behavior if regions overlap (use memmove for overlapping regions)
	 * @warning Destination and source must not overlap
	 */
	LIBACHERON void* memcpy(void *dest, const void *src, size_t count)
	{
		auto *d = static_cast<uint8_t *>(dest);
		const auto *s = static_cast<const uint8_t *>(src);
		if (count < 8 || ((reinterpret_cast<uintptr_t>(d) | reinterpret_cast<uintptr_t>(s)) & (sizeof(size_t) - 1)))
		{
			while (count--)
				*d++ = *s++;
			return dest;
		}

		auto *dw = reinterpret_cast<size_t *>(d);
		const auto *sw = reinterpret_cast<const size_t *>(s);
		while (count >= sizeof(size_t))
		{
			*dw++ = *sw++;
			count -= sizeof(size_t);
		}

		d = reinterpret_cast<uint8_t *>(dw);
		s = reinterpret_cast<const uint8_t *>(sw);
		while (count--)
			*d++ = *s++;

		return dest;
	}

	/**
	 * @brief Copy memory area, handling overlapping regions
	 *
	 * @param dest Pointer to destination memory area
	 * @param src Pointer to source memory area
	 * @param count Number of bytes to copy
	 * @return void* The original value of dest
	 *
	 * @note Uses word-aligned copy for non-overlapping regions
	 * @note For overlapping regions, copies in a way that preserves the source content
	 * @note Implementation-defined: If regions overlap, a direction-based copy is used
	 *       (forward or backward) depending on the relative positions of source and destination
	 */
	LIBACHERON void* memmove(void *dest, const void *src, size_t count)
	{
		auto *d = static_cast<uint8_t *>(dest);
		auto *s = static_cast<const uint8_t *>(src);

		if (d == s || count == 0)
			return dest;

		/* forward copy if they don't overlap */
		if (d < s || d >= s + count)
			return memcpy(dest, src, count);

		/* copy backward to avoid corruption otherwise */
		d += count;
		s += count;
		if (count < 8)
		{
			while (count--)
				*--d = *--s;
			return dest;
		}

		/* handle trailing bytes for alignment */
		size_t d_align = reinterpret_cast<uintptr_t>(d) & (sizeof(size_t) - 1);
		while (d_align--)
		{
			*--d = *--s;
			count--;
		}

		/* check if both pointers are now aligned */
		if (const bool aligned = !(reinterpret_cast<uintptr_t>(s) & (sizeof(size_t) - 1));
			aligned && count >= sizeof(size_t))
		{
			/* word-by-word backward for aligned pointers */
			const size_t words = count / sizeof(size_t);
			count &= sizeof(size_t) - 1;

			d -= words * sizeof(size_t);
			s -= words * sizeof(size_t);

			auto *dw = reinterpret_cast<size_t *>(d);
			const auto *sw = reinterpret_cast<const size_t *>(s);

			for (size_t i = 0; i < words; i++)
				dw[i] = sw[i];
		}

		/* handle remaining bytes */
		while (count--)
			*--d = *--s;

		return dest;
	}

	/**
	 * @brief Fill memory with a constant byte
	 *
	 * @param s Pointer to memory area
	 * @param c Value to be set (converted to unsigned char)
	 * @param count Number of bytes to be set
	 * @return void* The original value of s
	 *
	 * @note Uses word-aligned operations for sizes >= 8 bytes
	 * @note The input value 'c' is truncated to an unsigned char
	 * @note Implementation-defined: Creates a word-sized pattern by replicating the
	 *       byte value across all positions
	 */
	LIBACHERON void* memset(void *s, int c, size_t count)
	{
		auto *xs = static_cast<uint8_t *>(s);
		const auto b = static_cast<uint8_t>(c);
		if (count < 8)
		{
			while (count--)
				*xs++ = b;
			return s;
		}

		/* align to word boundary */
		size_t align = -reinterpret_cast<uintptr_t>(xs) & (sizeof(size_t) - 1);
		count -= align;
		while (align--)
			*xs++ = b;

		/* make word-sized pattern */
		size_t pattern = b;
		for (size_t i = 1; i < sizeof(size_t); i++)
			pattern |= (static_cast<size_t>(b) << (i * 8));

		auto *xw = reinterpret_cast<size_t *>(xs);
		while (count >= sizeof(size_t))
		{
			*xw++ = pattern;
			count -= sizeof(size_t);
		}

		/* fill the remaining bytes */
		xs = reinterpret_cast<uint8_t *>(xw);
		while (count--)
			*xs++ = b;

		return s;
	}
}
