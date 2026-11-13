#pragma once

namespace Turbo
{
	template <typename T>
	struct TRect2
	{
		TVec2<T> min;
		TVec2<T> max;
	};

	using IRect2 = TRect2<i32>;
	using URect2 = TRect2<u32>;
	using Rect2 = TRect2<float>;
	using DRect2 = TRect2<double>;
} // namespace Turbo
