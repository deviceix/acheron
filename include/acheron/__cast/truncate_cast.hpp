/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <limits>
#include <acheron/__libdef.hpp>

namespace ach
{
	template<typename T, typename U>
		requires std::is_integral_v<T>
			&& std::is_integral_v<U>
			&& sizeof(U) <= sizeof(T)
			&& (std::numeric_limits<U>::min() >= std::numeric_limits<T>::min()
				&& std::numeric_limits<U>::max() <= std::numeric_limits<T>::max())
	LIBACHERON constexpr T truncate_cast(T v)
	{
		return static_cast<T>(v);
	}
}
