#pragma once

#include <math.h>

namespace Turbo
{
	// ##################################################################################
	//                                   Basic math
	// ##################################################################################

	template <typename T>
	constexpr T Max(T a, T b)
	{
		return max(a, b);
	}

	template <typename T>
	constexpr T Min(T a, T b)
	{
		return min(a, b);
	}

	template <typename T>
	void Swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
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
