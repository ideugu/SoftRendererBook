#pragma once

#include "Color32.h"

struct LinearColor
{
public:
	FORCEINLINE LinearColor() = default;
	FORCEINLINE explicit LinearColor(float InR, float InG, float InB, float InA = 1.f) : _R(InR), _G(InG), _B(InB), _A(InA) {}
	FORCEINLINE explicit LinearColor(const Color32& InColor32)
	{
		_R = float(InColor32._R) * OneOver255;
		_G = float(InColor32._G) * OneOver255;
		_B = float(InColor32._B) * OneOver255;
		_A = float(InColor32._A) * OneOver255;
	}

	FORCEINLINE Color32 ToColor32(const bool bSRGB = false) const;

	FORCEINLINE LinearColor operator+(const LinearColor& InColor) const;
	FORCEINLINE LinearColor operator-(const LinearColor& InColor) const;
	FORCEINLINE LinearColor operator*(const LinearColor& InColor) const;
	FORCEINLINE LinearColor operator*(float InScalar) const;
	FORCEINLINE bool operator==(const LinearColor& InColor) const;
	FORCEINLINE bool operator!=(const LinearColor& InColor) const;

	FORCEINLINE bool EqualsInRange(const LinearColor& InColor, float InTolerance = KINDA_SMALL_NUMBER) const;

	static const float OneOver255;
	static const LinearColor Error;
	static const LinearColor White;
	static const LinearColor Black;
	static const LinearColor Gray;
	static const LinearColor Red;
	static const LinearColor Green;
	static const LinearColor Blue;
	static const LinearColor Yellow;
	static const LinearColor Cyan;
	static const LinearColor Magenta;

public:
	float _R = 0.f;
	float _G = 0.f;
	float _B = 0.f;
	float _A = 1.f;
};

FORCEINLINE Color32 LinearColor::ToColor32(const bool bSRGB) const
{
	float FloatR = Math::Clamp(_R, 0.f, 1.f);
	float FloatG = Math::Clamp(_G, 0.f, 1.f);
	float FloatB = Math::Clamp(_B, 0.f, 1.f);
	float FloatA = Math::Clamp(_A, 0.f, 1.f);

	return Color32(
		(int)(FloatR * 255.999f),
		(int)(FloatG * 255.999f),
		(int)(FloatB * 255.999f),
		(int)(FloatA * 255.999f)
	);
}

FORCEINLINE LinearColor LinearColor::operator+(const LinearColor& InColor) const
{
	return LinearColor(
		_R + InColor._R,
		_G + InColor._G,
		_B + InColor._B,
		_A + InColor._A
	);
}

FORCEINLINE LinearColor LinearColor::operator-(const LinearColor& InColor) const
{
	return LinearColor(
		_R - InColor._R,
		_G - InColor._G,
		_B - InColor._B,
		_A - InColor._A
	);
}

FORCEINLINE LinearColor LinearColor::operator*(const LinearColor& InColor) const
{
	return LinearColor(
		_R * InColor._R,
		_G * InColor._G,
		_B * InColor._B,
		_A * InColor._A
	);
}

FORCEINLINE LinearColor LinearColor::operator*(float InScalar) const
{
	return LinearColor(
		_R * InScalar,
		_G * InScalar,
		_B * InScalar,
		_A * InScalar
	);
}

FORCEINLINE bool LinearColor::operator==(const LinearColor& InColor) const
{
	return this->_R == InColor._R && this->_G == InColor._G && this->_B == InColor._B && this->_A == InColor._A;
}

FORCEINLINE bool LinearColor::operator!=(const LinearColor& InColor) const
{
	return this->_R != InColor._R || this->_G != InColor._G || this->_B != InColor._B || this->_A != InColor._A;
}

FORCEINLINE bool LinearColor::EqualsInRange(const LinearColor& InColor, float InTolerance) const
{
	return (Math::Abs(this->_R - InColor._R) < InTolerance) &&
		(Math::Abs(this->_G - InColor._G) < InTolerance) &&
		(Math::Abs(this->_B - InColor._B) < InTolerance) &&
		(Math::Abs(this->_A - InColor._A) < InTolerance);
}

