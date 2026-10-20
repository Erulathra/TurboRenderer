#pragma once

#include "Math/Math.h"

namespace Turbo
{
	// ##################################################################################
	//                                       TVec2
	// ##################################################################################

	template <typename T>
	struct TVec2
	{
		/* Data */
		T x;
		T y;

		/* ctors */
		constexpr TVec2()
			: x(0), y(0)
		{
		}

		constexpr TVec2(T inX, T inY)
			: x(inX), y(inY)
		{
		}

		constexpr TVec2(T val)
			: x(val), y(val)
		{
		}

		template <typename OtherT>
		constexpr TVec2(const TVec2<OtherT>& vec2)
			: x(vec2.x), y(vec2.y)
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

		/* Constants */
		inline static TVec2 One   = { 1.,  1.};
		inline static TVec2 Zero  = { 0.,  0.};

		inline static TVec2 Up    = { 1.,  0.};
		inline static TVec2 Down  = {-1.,  0.};
		inline static TVec2 Right = { 0.,  1.};
		inline static TVec2 Left  = { 0., -1.};
	};

	/* Static functions */
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

	/* Typedefs */
	using ivec2 = TVec2<i32>;
	using uvec2 = TVec2<u32>;
	using vec2  = TVec2<float>;
	using dvec2 = TVec2<double>;

	// ##################################################################################
	//                                       TVec3
	// ##################################################################################

	template <typename T>
	struct TVec3
	{
		/* Data */
		T x;
		T y;
		T z;

		/* ctors */
		constexpr TVec3()
			: x(0), y(0), z(0)
		{
		}

		constexpr TVec3(T inX, T inY, T inZ)
			: x(inX), y(inY), z(inZ)
		{
		}

		constexpr TVec3(T val)
			: x(val), y(val), z(val)
		{
		}

		template <typename OtherT>
		constexpr TVec3(const TVec3<OtherT>& vec2)
			: x(vec2.x), y(vec2.y), z(0)
		{
		}

		constexpr TVec3(const TVec2<T>& vec2, T inZ)
			: x(vec2.x), y(vec2.y), z(inZ)
		{
		}

		constexpr explicit operator TVec2<T>() const
		{
			return TVec2<T>{x, y};
		}

		/* Operators */
		constexpr TVec3& operator=(const TVec3& rhs) noexcept
		{
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;

			return *this;
		}

		constexpr bool operator==(const TVec3& rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z; }
		constexpr bool operator!=(const TVec3& rhs) const noexcept { return !(*this == rhs); }

		constexpr TVec3 operator-() const noexcept { return {-x, -y, -z}; }

		constexpr TVec3 operator-(const TVec3& rhs) const noexcept { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
		constexpr void operator-=(const TVec3& rhs) noexcept { *this = *this - rhs; }

		constexpr TVec3 operator+(const TVec3& rhs) const noexcept { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
		constexpr void operator+=(const TVec3& rhs) noexcept { *this = *this + rhs; }

		constexpr TVec3 operator*(const TVec3& rhs) const noexcept { return {x * rhs.x, y * rhs.y, z * rhs.z}; }
		constexpr void operator*=(const TVec3& rhs) noexcept { *this = *this * rhs; }

		constexpr TVec3 operator*(T rhs) const noexcept { return {x * rhs, y * rhs, z * rhs}; }
		constexpr void operator*=(T rhs) noexcept { *this = *this * rhs; }

		constexpr TVec3 operator/(const TVec3& rhs) const noexcept { return {x / rhs.x, y / rhs.y, z / rhs.z}; }
		constexpr void operator/=(const TVec3& rhs) noexcept { *this = *this / rhs; }

		constexpr TVec3 operator/(T rhs) const noexcept { return {x / rhs, y / rhs, z / rhs}; }
		constexpr void operator/=(T rhs) noexcept { *this = *this / rhs; }

		/* Constants */
		inline static TVec3 One     = { 1.,  1.,  1.};
		inline static TVec3 Zero    = { 0.,  0.,  0.};

		inline static TVec3 Up      = { 1.,  0.,  0.};
		inline static TVec3 Down    = {-1.,  0.,  0.};
		inline static TVec3 Right   = { 0.,  1.,  0.};
		inline static TVec3 Left    = { 0., -1.,  0.};
		inline static TVec3 Forward = { 0.,  0.,  1.};
		inline static TVec3 Back    = { 0.,  0., -1.};
	};

	/* Static functions */
	template<typename T>
	constexpr T lengthSq(const TVec3<T>& lhs) { return lhs.x * lhs.x + lhs.y * lhs.y + lhs.z * lhs.z; }

	template<typename T>
	constexpr T length(const TVec3<T>& lhs) { return Sqrt(lengthSq(lhs)); }

	template<typename T>
	constexpr T dot(const TVec3<T>& lhs, const TVec3<T>& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * lhs.z; }

	template <typename T>
	constexpr TVec3<T> cross(const TVec3<T>& lhs, const TVec3<T>& rhs)
	{
		return {
			 lhs.y * rhs.z - lhs.z * rhs.y,
			 lhs.z * rhs.x - lhs.x * rhs.z,
			 lhs.x * rhs.y - lhs.y * rhs.x,
		};
	}

	template<typename T>
	constexpr TVec3<T> normalize(const TVec3<T>& lhs) { return lhs / length(lhs); }

	/* Typedefs */
	using ivec3 = TVec3<i32>;
	using uvec3 = TVec3<u32>;
	using vec3  = TVec3<float>;
	using dvec3 = TVec3<double>;
} // namespace Turbo
