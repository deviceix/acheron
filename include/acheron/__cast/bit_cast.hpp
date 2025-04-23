/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <acheron/__libdef.hpp>
#include <acheron/__cstring/__memops.hpp>

namespace ach
{
	template<typename T, typename U>
		requires (std::is_trivially_copyable_v<T> &&
			(sizeof(T) == sizeof(U)))
	LIBACHERON constexpr T bit_cast(const U& src)
	{
		T dest;
		memcpy(&dest, &src, sizeof(T));
		return dest;
	}
}
