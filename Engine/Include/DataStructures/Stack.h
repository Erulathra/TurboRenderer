#pragma once

// NOTE(SS): Disable initializer_list for now. I will decide later if this required.
#define WITH_INITIALIZER_LIST 0

#if WITH_INITIALIZER_LIST
#include <initializer_list>
#endif // WITH_INITIALIZER_LIST

template <typename T, u32 capacity>
struct TStaticStack
{
	using Iterator = T*;

	T data[capacity];
	u32 size;

	TStaticStack()
		: size(0)
	{
	}

#if WITH_INITIALIZER_LIST
	constexpr TStaticStack(std::initializer_list<T>&& initializerList)
	{
		check(initializerList.size() <= capacity);

		size = initializerList.size();
		u32 i = 0;
		for (const T& element : initializerList)
		{
			data[i] = element;
			++i;
		}
	}

	constexpr TStaticStack& operator=(std::initializer_list<T>&& initializerList)
	{
		check(initializerList.size() <= capacity);

		size = initializerList.size();
		u32 i = 0;
		for (const T& element : initializerList)
		{
			data[i] = element;
			++i;
		}

		return *this;
	}
#endif // WITH_INITIALIZER_LIST

	T& Push(const T& newElement)
	{
		check(size + 1 <= capacity);
		++size;

		data[size - 1] = newElement;

		return data[size - 1];
	}

	T& Push()
	{
		check(size + 1 <= capacity);
		++size;

		return data[size - 1];
	}

	void Pop()
	{
		check(size > 0);
		--size;
	}

	void SetSize(u32 newSize)
	{
		check(newSize <= capacity);
		size = newSize;
	}

	void Empty() { size = 0; }

	bool IsEmpty() { return size == 0; }

	T& First() { return (*this)[0]; }
	const T& First() const { return (*this)[0]; }
	T& Top() { return (*this)[size - 1]; }
	const T& Top() const { return (*this)[size - 1]; }

	/* Operators */
	T& operator[](u32 index)
	{
		check(index < size);
		return data[index];
	}

	const T& operator[](u32 index) const
	{
		check(index < size);
		return data[index];
	}
	/* Iterators () */
	Iterator begin() { return data; }
	Iterator end() { return data + size; }
};
