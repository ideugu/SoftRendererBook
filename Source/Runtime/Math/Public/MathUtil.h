#pragma once

#include <intrin.h>
#include <math.h>
#include "Platform.h"

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

	template<class T>
	static constexpr FORCEINLINE T Square(const T InNum)
	{
		return InNum * InNum;
	}

	static constexpr FORCEINLINE float Deg2Rad(float InDegree)
	{
		return InDegree * PI / 180.f;
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
			OutSin = sinf(rad);
			OutCos = cosf(rad);
		}
	}
};

