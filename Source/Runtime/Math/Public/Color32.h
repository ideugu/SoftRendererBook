#pragma once

#include "MathUtil.h"

struct Color32
{
public:
	FORCEINLINE Color32() : _R(0), _G(0), _B(0), _A(0) { }
	FORCEINLINE explicit Color32(BYTE InR, BYTE InG, BYTE InB, BYTE InA = 255) : _B(InB), _G(InG), _R(InR), _A(InA) { }
	FORCEINLINE explicit Color32(UINT32 InColor) { GetColorRef() = InColor; }

	FORCEINLINE const UINT32& GetColorRef() const { return *((UINT32*)this); }
	FORCEINLINE UINT32& GetColorRef() { return *((UINT32*)this); }

	FORCEINLINE bool operator==(const Color32& InC) const;
	FORCEINLINE bool operator!=(const Color32& InC) const;
	FORCEINLINE void operator+=(const Color32& InC);

	static const Color32 Error;

public:
	union 
	{ 
		struct 
		{ 
			BYTE _B, _G, _R, _A; 
		};

		BYTE _Dummy; 
	};
};

FORCEINLINE bool Color32::operator==(const Color32& InC) const
{
	return GetColorRef() == InC.GetColorRef();
}

FORCEINLINE bool Color32::operator!=(const Color32& InC) const
{
	return GetColorRef() != InC.GetColorRef();
}

FORCEINLINE void Color32::operator+=(const Color32& InC)
{
	_R = (BYTE)Math::Clamp((BYTE)_R + (BYTE)InC._R, 0, 255);
	_G = (BYTE)Math::Clamp((BYTE)_G + (BYTE)InC._G, 0, 255);
	_B = (BYTE)Math::Clamp((BYTE)_B + (BYTE)InC._B, 0, 255);
	_A = (BYTE)Math::Clamp((BYTE)_A + (BYTE)InC._A, 0, 255);
}

