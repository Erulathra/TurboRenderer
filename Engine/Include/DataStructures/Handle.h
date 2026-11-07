#pragma once

namespace Turbo
{
	struct Handle
	{
		/* TypeDefs */
		using IndexType = u64;
		using GenType = u32;

		/* Constants */
		constexpr static IndexType indexMask = 0xFFFFFFFF;
		constexpr static IndexType genMask = 0xFFFFFFFF;
		constexpr static IndexType indexMaskBits = 32;
		constexpr static IndexType nilIndex = 0;

		FORCEINLINE constexpr static Handle Nil() { return Handle(nilIndex); };

		/* Data */
		IndexType mIndexAndGen = 0;

		/* Api */
		FORCEINLINE IndexType GetIndex() const { return mIndexAndGen & indexMask; }
		FORCEINLINE IndexType GetGen() const { return (mIndexAndGen >> indexMaskBits) & genMask; }
		FORCEINLINE operator bool() const { return GetIndex() != nilIndex; }
		FORCEINLINE bool operator==(const Handle& rhs) const { return mIndexAndGen == rhs.mIndexAndGen; }

		FORCEINLINE static IndexType CreateIndex(IndexType index, IndexType gen)
		{
			check(index <= indexMask && gen <= genMask);
			return (gen << indexMaskBits) | index;
		}
	};

	/* THandle is used to improve type safety */
	template <typename T>
	struct THandle final : public Handle
	{
		FORCEINLINE static THandle<T> Create(u32 index, u32 gen) { return {CreateIndex(index, gen)}; }
		FORCEINLINE static THandle<T> Nil() { return THandle<T>(); }
	};
} // namespace Turbo
