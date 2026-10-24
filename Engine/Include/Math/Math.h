#pragma once

#include <math.h>

#define SMALL_NUMBER 1e-4
#define VERY_SMALL_NUMBER 1e-8

#define TE_PI (3.14159265358979323846264)
#define TE_TAU (6.28318530717958647692528)

#define TE_DEG_TO_RAD (0.01745329251994329576923)
#define TE_RAD_TO_DEG (57.2957795130823208767981)

namespace Turbo
{
	// ##################################################################################
	//                                   Basic math
	// ##################################################################################

	template <typename T>
	FORCEINLINE constexpr T Max(T a, T b)
	{
		return max(a, b);
	}

	template <typename T>
	FORCEINLINE constexpr T Min(T a, T b)
	{
		return min(a, b);
	}

	template <typename T>
	FORCEINLINE constexpr void Swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	template <typename T>
	FORCEINLINE constexpr T Sqrt(T x)
	{
		return sqrt(x);
	}

	template <typename T>
	FORCEINLINE constexpr T Abs(T x)
	{
		return abs(x);
	}

	template <typename T>
	FORCEINLINE constexpr T Sin(T x)
	{
		return sin(x);
	}

	template <typename T>
	FORCEINLINE constexpr T Cos(T x)
	{
		return cos(x);
	}

	template <typename T>
	FORCEINLINE void SinCos(T x, T& outSin, T& outCos)
	{
		// Use sin and cos
		sincos(x, &outSin, &outCos);
	}

	template<>
	inline void SinCos<float>(float x, float& outSin, float& outCos)
	{
		sincosf(x, &outSin, &outCos);
	}

	template<typename T>
	FORCEINLINE constexpr T DegToRad(T deg)
	{
		return deg * (T)TE_DEG_TO_RAD;
	}

	template<typename T>
	FORCEINLINE constexpr T RadToDeg(T deg)
	{
		return deg * (T)TE_RAD_TO_DEG;
	}

	// ##################################################################################
	//                               Integer Truncation
	// ##################################################################################

	template <typename SrcT>
	u32 TruncToU32(SrcT x)
	{
		check(x >= 0 && x <= U32_MAX);
		return (u32)(x);
	}

	template <typename SrcT>
	u64 TruncToU64(SrcT x)
	{
		check(x >= 0 && x <= U64_MAX);
		return (u64)(x);
	}

	template <typename SrcT>
	i32 TruncToI32(SrcT x)
	{
		check(x >= I32_MIN && x <= I32_MAX);
		return (i32)(x);
	}

	template <typename SrcT>
	i64 TruncToI64(SrcT x)
	{
		check(x >= I64_MIN && x <= I64_MAX);
		return (i64)(x);
	}
} // namespace Turbo::Math
