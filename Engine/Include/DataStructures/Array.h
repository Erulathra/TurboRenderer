#pragma once

#include <string.h>

namespace Turbo
{
	template<typename T>
	struct TArrayView
	{
		/* Data */
		T* start = nullptr;
		u64 size = 0;

		/* Ctors */
		TArrayView(T* inStart, u64 inSize) : start(inStart), size(inSize) {}
		TArrayView(T* inStart, T* inEnd) : start(inStart), size(u64((iPtr)inEnd - (iPtr)inStart)) { check(inStart < inEnd); }

		/* range-based-for requiments */
		FORCEINLINE T* begin() { return start; }
		FORCEINLINE T* end() { return start + size; }
	};

	using ByteView = TArrayView<u8>;

	FORCEINLINE ByteView MakeByteView(LiteralString literalString)
	{
		return {(u8*)literalString, strlen(literalString)};
	}

	FORCEINLINE ByteView MakeByteView(void* data, u64 size)
	{
		return {(u8*)data, size};
	}

	template<typename T>
	FORCEINLINE ByteView MakeByteView(T* data)
	{
		return {(u8*)data, sizeof(&data)};
	}

	template<typename T>
	FORCEINLINE ByteView MakeByteArrayView(T* data, u64 size)
	{
		return {(u8*)data, sizeof(&data) * size};
	}
}
