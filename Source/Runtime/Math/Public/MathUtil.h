#pragma once

namespace CK
{

struct Math
{
	static const float PI;
	static const float TwoPI;
	static const float HalfPI;
	static const float InvPI;
	static const int IntMin;
	static const int IntMax;

	static FORCEINLINE int TruncToInt(float InFloat)
	{
		return (int)InFloat;
	}

	static FORCEINLINE int RountToInt(float InFloat)
	{
		return TruncToInt(roundf(InFloat));
	}

	static FORCEINLINE int FloorToInt(float InFloat)
	{
		return TruncToInt(floorf(InFloat));
	}

	static FORCEINLINE int CeilToInt(float InFloat)
	{
		return TruncToInt(ceilf(InFloat));
	}

	static FORCEINLINE bool EqualsInTolerance(float InFloat1, float InFloat2, float InTolerance = KINDA_SMALL_NUMBER)
	{
		return Math::Abs(InFloat1 - InFloat2) <= InTolerance;
	}

	template< class T>
	static FORCEINLINE T Lerp(const T& InSrc, const T& InDest, float InAlpha)
	{
		return (T)(InSrc + InAlpha * (InDest - InSrc));
	}

	template<class T>
	static constexpr FORCEINLINE T Square(const T InNum)
	{
		return InNum * InNum;
	}

	static constexpr FORCEINLINE float Deg2Rad(float InDegree)
	{
		return InDegree * PI / 180.f;
	}

	static constexpr FORCEINLINE float Rad2Deg(float InRadian)
	{
		return InRadian * 180.f * InvPI;
	}

	template<class T>
	static constexpr FORCEINLINE T Max(const T A, const T B)
	{
		return (A >= B) ? A : B;
	}

	template<class T>
	static constexpr FORCEINLINE T Min(const T A, const T B)
	{
		return (A <= B) ? A : B;
	}

	template<class T>
	static constexpr FORCEINLINE T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	template<class T>
	static constexpr FORCEINLINE T Clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}

	static FORCEINLINE void GetSinCos(float& OutSin, float& OutCos, float InDegree)
	{
		if (InDegree == 0.f)
		{
			OutSin = 0.f;
			OutCos = 1.f;
		}
		else if (InDegree == 90.f)
		{
			OutSin = 1.f;
			OutCos = 0.f;
		}
		else if (InDegree == 180.f)
		{
			OutSin = 0.f;
			OutCos = -1.f;
		}
		else if (InDegree == 270.f)
		{
			OutSin = -1.f;
			OutCos = 0.f;
		}
		else
		{
			float rad = Math::Deg2Rad(InDegree);

			// Copied from UE4 Source Code
			// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
			float quotient = (InvPI * 0.5f) * rad;
			if (rad >= 0.0f)
			{
				quotient = (float)((int)(quotient + 0.5f));
			}
			else
			{
				quotient = (float)((int)(quotient - 0.5f));
			}
			float y = rad - (2.0f * PI) * quotient;

			// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
			float sign;
			if (y > HalfPI)
			{
				y = PI - y;
				sign = -1.0f;
			}
			else if (y < -HalfPI)
			{
				y = -PI - y;
				sign = -1.0f;
			}
			else
			{
				sign = +1.0f;
			}

			float y2 = y * y;

			// 11-degree minimax approximation
			OutSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

			// 10-degree minimax approximation
			float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
			OutCos = sign * p;
		}
	}

	static FORCEINLINE float FMod(float X, float Y)
	{
		if (fabsf(Y) <= SMALL_NUMBER)
		{
			return 0.f;
		}

		const float quotient = (float)TruncToInt(X / Y);
		float intPortion = Y * quotient;
		if (fabsf(intPortion) > fabsf(X))
		{
			intPortion = X;
		}

		return (X - intPortion);
	}
};

}