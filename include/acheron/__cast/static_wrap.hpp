/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#pragma once

#include <acheron/__libdef.hpp>

namespace ach
{
	template<typename T, typename U>
	LIBACHERON constexpr U cast(T v)
	{
		return static_cast<U>(v);
	}
}
