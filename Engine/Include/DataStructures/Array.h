#pragma once

#include <string.h>

namespace Turbo
{
	template<typename T>
	struct TArrayView
	{
		/* Data */
		T* mStart = nullptr;
		u64 mSize = 0;

		/* Ctors */
		TArrayView(T* start, u64 size) : mStart(start), mSize(size) {}
		TArrayView(T* start, T* end) : mStart(start), mSize(u64((iPtr)end - (iPtr)start)) { check(start < end); }

		/* range-based-for requiments */
		FORCEINLINE T* begin() { return mStart; }
		FORCEINLINE T* end() { return mStart + mSize; }
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
