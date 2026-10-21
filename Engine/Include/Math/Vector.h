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
		constexpr explicit TVec2(const TVec2<OtherT>& vec2)
			: x(vec2.x), y(vec2.y)
		{
		}

		/* Operators */
		constexpr TVec2& operator=(const TVec2& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;

			return *this;
		}

		constexpr bool operator==(const TVec2& rhs) const { return x == rhs.x && y == rhs.y; }
		constexpr bool operator!=(const TVec2& rhs) const { return !(*this == rhs); }

		constexpr TVec2 operator-() const { return TVec2{-x, -y}; }

		constexpr TVec2 operator-(const TVec2& rhs) const { return TVec2{x - rhs.x, y - rhs.y}; }
		constexpr void operator-=(const TVec2& rhs) { *this = *this - rhs; }

		constexpr TVec2 operator+(const TVec2& rhs) const { return TVec2{x + rhs.x, y + rhs.y}; }
		constexpr void operator+=(const TVec2& rhs) { *this = *this + rhs; }

		constexpr TVec2 operator*(const TVec2& rhs) const { return TVec2{x * rhs.x, y * rhs.y}; }
		constexpr void operator*=(const TVec2& rhs) { *this = *this * rhs; }

		constexpr TVec2 operator*(T rhs) const { return TVec2{x * rhs, y * rhs}; }
		constexpr void operator*=(T rhs) { *this = *this * rhs; }

		constexpr TVec2 operator/(const TVec2& rhs) const { return TVec2{x / rhs.x, y / rhs.y}; }
		constexpr void operator/=(const TVec2& rhs) { *this = *this / rhs; }

		constexpr TVec2 operator/(T rhs) const { return TVec2{x / rhs, y / rhs}; }
		constexpr void operator/=(T rhs) { *this = *this / rhs; }

		/* Static functions */
		constexpr static T LengthSq(const TVec2& lhs) { return lhs.x * lhs.x + lhs.y * lhs.y; }
		constexpr static T Length(const TVec2& lhs) { return Sqrt(LengthSq(lhs)); }
		constexpr static T Dot(const TVec2& lhs, const TVec2& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }
		constexpr static T Cross(const TVec2& lhs, const TVec2& rhs) { return lhs.x * rhs.y - lhs.y * rhs.x; }
		constexpr static TVec2<T> Normalize(const TVec2& lhs) { return lhs / Length(lhs); }

		/* Constants */
		FORCEINLINE constexpr static TVec2 One()   { return TVec2{ 1.,  1.}; }
		FORCEINLINE constexpr static TVec2 Zero()  { return TVec2{ 0.,  0.}; }

		FORCEINLINE constexpr static TVec2 Up()    { return TVec2{ 0.,  1.}; }
		FORCEINLINE constexpr static TVec2 Down()  { return TVec2{ 0., -1.}; }
		FORCEINLINE constexpr static TVec2 Right() { return TVec2{ 1.,  0.}; }
		FORCEINLINE constexpr static TVec2 Left()  { return TVec2{-1.,  0.}; }
	};



	/* Typedefs */
	using IVec2 = TVec2<i32>;
	using UVec2 = TVec2<u32>;
	using Vec2  = TVec2<float>;
	using DVec2 = TVec2<double>;

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

		constexpr TVec3(const TVec2<T>& vec2, T inZ)
			: x(vec2.x), y(vec2.y), z(inZ)
		{
		}

		constexpr explicit operator TVec2<T>() const
		{
			return TVec2<T>{x, y};
		}

		template <typename OtherT>
		constexpr explicit TVec3(const TVec3<OtherT>& rhs)
			: x(rhs.x), y(rhs.y), z(rhs.z)
		{
		}

		/* Operators */
		constexpr TVec3& operator=(const TVec3& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;

			return *this;
		}

		constexpr bool operator==(const TVec3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
		constexpr bool operator!=(const TVec3& rhs) const { return !(*this == rhs); }

		constexpr TVec3 operator-() const { return TVec3{-x, -y, -z}; }

		constexpr TVec3 operator-(const TVec3& rhs) const { return TVec3{x - rhs.x, y - rhs.y, z - rhs.z}; }
		constexpr void operator-=(const TVec3& rhs) { *this = *this - rhs; }

		constexpr TVec3 operator+(const TVec3& rhs) const { return TVec3{x + rhs.x, y + rhs.y, z + rhs.z}; }
		constexpr void operator+=(const TVec3& rhs) { *this = *this + rhs; }

		constexpr TVec3 operator*(const TVec3& rhs) const { return TVec3{x * rhs.x, y * rhs.y, z * rhs.z}; }
		constexpr void operator*=(const TVec3& rhs) { *this = *this * rhs; }

		constexpr TVec3 operator*(T rhs) const { return TVec3{x * rhs, y * rhs, z * rhs}; }
		constexpr void operator*=(T rhs) { *this = *this * rhs; }

		constexpr TVec3 operator/(const TVec3& rhs) const { return TVec3{x / rhs.x, y / rhs.y, z / rhs.z}; }
		constexpr void operator/=(const TVec3& rhs) { *this = *this / rhs; }

		constexpr TVec3 operator/(T rhs) const { return TVec3{x / rhs, y / rhs, z / rhs}; }
		constexpr void operator/=(T rhs) { *this = *this / rhs; }

		/* Static functions */
		constexpr static T LengthSq(const TVec3& lhs) { return lhs.x * lhs.x + lhs.y * lhs.y + lhs.z * lhs.z; }
		constexpr static T Length(const TVec3& lhs) { return Sqrt(LengthSq(lhs)); }
		constexpr static T Dot(const TVec3& lhs, const TVec3& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }

		constexpr static TVec3 Cross(const TVec3& lhs, const TVec3& rhs)
		{
			return {
				 lhs.y * rhs.z - lhs.z * rhs.y,
				 lhs.z * rhs.x - lhs.x * rhs.z,
				 lhs.x * rhs.y - lhs.y * rhs.x,
			};
		}

		constexpr static TVec3 Normalize(const TVec3& lhs) { return lhs / Length(lhs); }

		/* Constants */
		FORCEINLINE constexpr static TVec3 One()     { return TVec3{ 1.,  1.,  1.}; }
		FORCEINLINE constexpr static TVec3 Zero()    { return TVec3{ 0.,  0.,  0.}; }

		FORCEINLINE constexpr static TVec3 Up()      { return TVec3{ 0.,  1.,  0.}; }
		FORCEINLINE constexpr static TVec3 Down()    { return TVec3{ 0., -1.,  0.}; }
		FORCEINLINE constexpr static TVec3 Right()   { return TVec3{ 1.,  0.,  0.}; }
		FORCEINLINE constexpr static TVec3 Left()    { return TVec3{-1.,  0.,  0.}; }
		FORCEINLINE constexpr static TVec3 Forward() { return TVec3{ 0.,  0.,  1.}; }
		FORCEINLINE constexpr static TVec3 Back()    { return TVec3{ 0.,  0., -1.}; }
	};


	/* Typedefs */
	using IVec3 = TVec3<i32>;
	using UVec3 = TVec3<u32>;
	using Vec3  = TVec3<float>;
	using DVec3 = TVec3<double>;

	// ##################################################################################
	//                                       TVec4
	// ##################################################################################

	template <typename T>
	struct TVec4
	{
		/* Data */
		T x;
		T y;
		T z;
		T w;

		/* ctors */
		constexpr TVec4()
			: x(0), y(0), z(0), w(0)
		{
		}

		constexpr TVec4(T inX, T inY, T inZ, T inW)
			: x(inX), y(inY), z(inZ), w(inW)
		{
		}

		constexpr TVec4(T val)
			: x(val), y(val), z(val), w(val)
		{
		}

		constexpr TVec4(const TVec2<T>& vec2, T inZ, T inW)
			: x(vec2.x), y(vec2.y), z(inZ), w(inW)
		{
		}

		constexpr TVec4(const TVec3<T>& vec3, T inW)
			: x(vec3.x), y(vec3.y), z(vec3.z), w(inW)
		{
		}

		constexpr explicit operator TVec2<T>() const
		{
			return TVec2<T>{x, y};
		}

		constexpr explicit operator TVec3<T>() const
		{
			return TVec3<T>{x, y, z};
		}

		template <typename OtherT>
		constexpr explicit TVec4(const TVec4<OtherT>& rhs)
			: x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
		{
		}

		/* Operators */
		constexpr TVec4& operator=(const TVec4& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;
			this->w = rhs.w;

			return *this;
		}

		constexpr bool operator==(const TVec4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
		constexpr bool operator!=(const TVec4& rhs) const { return !(*this == rhs); }

		constexpr TVec4 operator-() const { return TVec4{-x, -y, -z, -w}; }

		constexpr TVec4 operator-(const TVec4& rhs) const { return TVec4{x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w}; }
		constexpr void operator-=(const TVec4& rhs) { *this = *this - rhs; }

		constexpr TVec4 operator+(const TVec4& rhs) const { return TVec4{x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w}; }
		constexpr void operator+=(const TVec4& rhs) { *this = *this + rhs; }

		constexpr TVec4 operator*(const TVec4& rhs) const { return TVec4{x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w}; }
		constexpr void operator*=(const TVec4& rhs) { *this = *this * rhs; }

		constexpr TVec4 operator*(T rhs) const { return TVec4{x * rhs, y * rhs, z * rhs, w * rhs}; }
		constexpr void operator*=(T rhs) { *this = *this * rhs; }

		constexpr TVec4 operator/(const TVec4& rhs) const { return TVec4{x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w}; }
		constexpr void operator/=(const TVec4& rhs) { *this = *this / rhs; }

		constexpr TVec4 operator/(T rhs) const { return TVec4{x / rhs, y / rhs, z / rhs, w / rhs}; }
		constexpr void operator/=(T rhs) { *this = *this / rhs; }

		/* Static functions */
		constexpr static T LengthSq(const TVec4& lhs) { return lhs.x * lhs.x + lhs.y * lhs.y + lhs.z * lhs.z + lhs.w * lhs.w; }
		constexpr static T Length(const TVec4& lhs) { return Sqrt(LengthSq(lhs)); }
		constexpr static TVec4 Normalize(const TVec4& lhs) { return lhs / Length(lhs); }

		/* Constants */
		FORCEINLINE constexpr static TVec4 One()  { return TVec4{ 1.,  1.,  1., 1.}; }
		FORCEINLINE constexpr static TVec4 Zero() { return TVec4{ 0.,  0.,  0., 0.}; }
	};

	/* Typedefs */
	using IVec4 = TVec4<i32>;
	using UVec4 = TVec4<u32>;
	using Vec4  = TVec4<float>;
	using DVec4 = TVec4<double>;
} // namespace Turbo
