#pragma once

#include "Math.h"
#include "Vector.h"

namespace Turbo
{
	template <typename T>
	struct alignas(16) TQuat
	{
		/* Data */
		T w;
		TVec3<T> v;

		/* CTors */
		TQuat(T inW, const TVec3<T>& inV)
			 : w(inW)
			 , v(inV)
		{
		}

		constexpr static TQuat FromAxisAngle(T radians, const TVec3<T>& normalizedAxis)
		{
			check(TVec3<T>::IsNormalized(normalizedAxis));

			const T halfAngle = radians / (T)2;
			T sinHalfAngle, cosHalfAngle;
			SinCos(halfAngle, sinHalfAngle, cosHalfAngle);

			return {
				cosHalfAngle,
				{
					sinHalfAngle * normalizedAxis.x,
					sinHalfAngle * normalizedAxis.y,
					sinHalfAngle * normalizedAxis.z,
				},
			};
		}

		/* Operators */
		constexpr TQuat& operator=(const TQuat& rhs)
		{
			w = rhs.w;
			v = rhs.v;

			return *this;
		}

		constexpr bool operator==(const TQuat& rhs) const { return w == rhs.w && v == rhs.v; }
		constexpr bool operator!=(const TQuat& rhs) const { return !(*this == rhs); }

		constexpr TQuat operator*(const TQuat& rhs) const
		{
			mathDiagnosticCheck(IsNormalized(*this) && IsNormalized(rhs));
			return {
				w * rhs.w - TVec3<T>::Dot(v, rhs.v),
				w * rhs.v + rhs.w * v + TVec3<T>::Cross(v, rhs.v),
			};
		}
		constexpr void operator*=(const TQuat& rhs) { *this = *this * rhs; }

		constexpr TVec3<T> RotateVector(const TVec3<T>& p)
		{
			mathDiagnosticCheck(IsNormalized(*this));
			return ((T)2 * w * w - (T)1) * p + (T)2 * TVec3<T>::Dot(v, p) * v + (T)2 * w * TVec3<T>::Cross(v, p);
		}

		/* Statics */
		constexpr static TQuat Normalize(const TQuat& rhs)
		{
			const T lengthSq = rhs.w * rhs.w + TVec3<T>::LengthSq(rhs.v);
			const T invLength = (T)1 / Sqrt(lengthSq);
			return {rhs.w * invLength, rhs.v * invLength};
		}

		constexpr static TQuat Inverse(const TQuat& rhs)
		{
			mathDiagnosticCheck(IsNormalized(rhs));
			return TQuat{rhs.w, -rhs.v};
		}

		constexpr static bool IsNormalized(const TQuat& rhs, T epsilon = SMALL_NUMBER)
		{
			return Abs((rhs.w * rhs.w + TVec3<T>::LengthSq(rhs.v)) - (T)1) < epsilon;
		}

		/*
			TODO(SS):
				* MatrixToQuat conversion
		*/

		/* Static */
		FORCEINLINE constexpr static TQuat Identity() { return TQuat{(T)1, {(T)0, (T)0, (T)0}}; }
	};

	/* Typedefs */
	using Quat = TQuat<float>;
	using DQuat = TQuat<double>;
} // namespace Turbo

#undef INLINE_MULTIPLICATION
