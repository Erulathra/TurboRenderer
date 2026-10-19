#pragma once

#include "Math/Math.h"

namespace Turbo
{
	template <typename T>
	struct TVec2
	{
		/* Data */
		T x;
		T y;

		/* ctors */
		constexpr TVec2()
			: x(0)
			, y(0)
		{
		}

		constexpr TVec2(T inX, T inY)
			: x(inX)
			, y(inY)
		{
		}

		constexpr TVec2(T val)
			: x(val)
			, y(val)
		{
		}

		template <typename OtherT>
		constexpr TVec2(const TVec2<OtherT>& vec2)
			: x(vec2.x)
			, y(vec2.y)
		{
		}

		/* Operators */
		constexpr TVec2& operator=(const TVec2& rhs) noexcept
		{
			this->x = rhs.x;
			this->y = rhs.y;

			return *this;
		}

		constexpr bool operator==(const TVec2& rhs) const noexcept { return x == rhs.x && y == rhs.y; }
		constexpr bool operator!=(const TVec2& rhs) const noexcept { return !(*this == rhs); }

		constexpr TVec2 operator-() const noexcept { return {-x, -y}; }

		constexpr TVec2 operator-(const TVec2& rhs) const noexcept { return {x - rhs.x, y - rhs.y}; }
		constexpr void operator-=(const TVec2& rhs) noexcept { *this = *this - rhs; }

		constexpr TVec2 operator+(const TVec2& rhs) const noexcept { return {x + rhs.x, y + rhs.y}; }
		constexpr void operator+=(const TVec2& rhs) noexcept { *this = *this + rhs; }

		constexpr TVec2 operator*(const TVec2& rhs) const noexcept { return {x * rhs.x, y * rhs.y}; }
		constexpr void operator*=(const TVec2& rhs) noexcept { *this = *this * rhs; }

		constexpr TVec2 operator*(T rhs) const noexcept { return {x * rhs, y * rhs}; }
		constexpr void operator*=(T rhs) noexcept { *this = *this * rhs; }

		constexpr TVec2 operator/(const TVec2& rhs) const noexcept { return {x / rhs.x, y / rhs.y}; }
		constexpr void operator/=(const TVec2& rhs) noexcept { *this = *this / rhs; }

		constexpr TVec2 operator/(T rhs) const noexcept { return {x / rhs, y / rhs}; }
		constexpr void operator/=(T rhs) noexcept { *this = *this / rhs; }
	};

	template<typename T>
	constexpr T lengthSq(const TVec2<T>& lhs) { return lhs.x * lhs.x + lhs.y * lhs.y; }

	template<typename T>
	constexpr T length(const TVec2<T>& lhs) { return Sqrt(lengthSq(lhs)); }

	template<typename T>
	constexpr T dot(const TVec2<T>& lhs, const TVec2<T>& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }

	template<typename T>
	constexpr T cross(const TVec2<T>& lhs, const TVec2<T>& rhs) { return lhs.x * rhs.y - lhs.y * rhs.x; }

	template<typename T>
	constexpr TVec2<T> normalize(const TVec2<T>& lhs) { return lhs / length(lhs); }

	using ivec2 = TVec2<i32>;
	using uvec2 = TVec2<u32>;
	using vec2 = TVec2<float>;
	using dvec2 = TVec2<double>;

} // namespace Turbo
