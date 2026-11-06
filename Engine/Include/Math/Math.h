#pragma once

#include <math.h>

#include "DataStructures/Array.h"

namespace Turbo
{
	// ##################################################################################
	//                                     Basic math
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
	//                                 Integer Truncation
	// ##################################################################################

	template <typename SrcT>
	u16 TruncToU16(SrcT x)
	{
		check(x >= 0 && x <= U16_MAX);
		return (u16)(x);
	}

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
	i16 TruncToI16(SrcT x)
	{
		check(x >= I16_MIN && x <= I16_MAX);
		return (i16)(x);
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

	// ##################################################################################
	//                                   Hash functions
	// ##################################################################################

	inline u64 FNVHash(ByteView byteView)
	{
		// SOURCE: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
		u64 h = 0xcbf29ce484222325ul;
		for (u8 byte : byteView)
		{
			h ^= byte;
			h *= 16777619;
		}
		return h;
	}
} // namespace Turbo
