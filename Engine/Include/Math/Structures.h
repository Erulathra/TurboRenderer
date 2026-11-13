#pragma once

#include "Math.h"

namespace Turbo
{
	template <typename T>
	struct TRect2
	{
		TVec2<T> min;
		TVec2<T> max;

		constexpr TRect2<T>()
			: min()
			, max()
		{
		}

		constexpr TRect2<T>(const TVec2<T>& a, const TVec2<T>& b)
		{
			min = {Min(a.x, b.x), Min(a.y, b.y)};
			max = {Max(a.x, b.x), Max(a.y, b.y)};
		}

		FORCEINLINE constexpr TVec2<T> Size() const { return max - min; }
		FORCEINLINE constexpr TVec2<T> Center() const { return (min + max) / (T)2; }
	};

	using IRect2 = TRect2<i32>;
	using URect2 = TRect2<u32>;
	using Rect2 = TRect2<float>;
	using DRect2 = TRect2<double>;
} // namespace Turbo
