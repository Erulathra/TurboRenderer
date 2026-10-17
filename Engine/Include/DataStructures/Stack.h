#pragma once

#include <initializer_list>

template <typename T, u32 capacity>
struct TStaticStack
{
	using Iterator = T*;

	T mData[capacity];
	u32 mSize;

	TStaticStack()
		: mSize(0)
	{
	}

	constexpr TStaticStack(std::initializer_list<T>&& initializerList)
	{
		check(initializerList.size() <= capacity);

		mSize = initializerList.size();
		u32 i = 0;
		for (const T& element : initializerList)
		{
			mData[i] = element;
			++i;
		}
	}

	constexpr TStaticStack& operator=(std::initializer_list<T>&& initializerList)
	{
		check(initializerList.size() <= capacity);

		mSize = initializerList.size();
		u32 i = 0;
		for (const T& element : initializerList)
		{
			mData[i] = element;
			++i;
		}

		return *this;
	}

	T& Push(const T& newElement)
	{
		check(mSize + 1 <= capacity);
		++mSize;

		mData[mSize - 1] = newElement;

		return mData[mSize - 1];
	}

	T& Push()
	{
		check(mSize + 1 <= capacity);
		++mSize;

		return mData[mSize - 1];
	}

	void Pop()
	{
		check(mSize > 0);
		--mSize;
	}

	void SetSize(u32 newSize)
	{
		check(newSize <= capacity);
		mSize = newSize;
	}

	void Empty() { mSize = 0; }

	bool IsEmpty() { return mSize == 0; }

	/* Operators */
	T& operator[](u32 index)
	{
		check(index < mSize);
		return mData[index];
	}

	const T& operator[](u32 index) const
	{
		check(index < mSize);
		return mData[index];
	}

	/* Iterators () */
	Iterator begin() { return mData; }
	Iterator end() { return mData + mSize; }
};
