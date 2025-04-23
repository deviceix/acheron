/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <type_traits>
#include <acheron/__libdef.hpp>

namespace ach
{
	template<typename T, typename U>
		requires(std::is_pointer_v<T> &&
				 std::is_pointer_v<U>)
	LIBACHERON constexpr T aligned_cast(U ptr)
	{
		using Target = std::remove_pointer_t<T>;
		constexpr auto alignment = alignof(Target);
		static_assert((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0,
						  "[ach::aligned_cast]: pointer is not properly aligned for target type");

		return reinterpret_cast<T>(ptr);
	}
}
