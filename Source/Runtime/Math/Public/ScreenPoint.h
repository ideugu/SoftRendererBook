#pragma once

#include "Vector2.h"

struct ScreenPoint
{
public:
	FORCEINLINE ScreenPoint() = default;
	FORCEINLINE explicit ScreenPoint(int InX, int InY) : _X(InX), _Y(InY) { }
	FORCEINLINE explicit ScreenPoint(float InX, float InY) : _X(Math::FloorToInt(InX)), _Y(Math::FloorToInt(InY)) { }
	FORCEINLINE explicit ScreenPoint(const Vector2& InPos) : ScreenPoint(InPos._X, InPos._Y) {}

	ScreenPoint GetHalf() { return ScreenPoint(Math::FloorToInt(0.5f * _X), Math::FloorToInt(0.5f * _Y)); }
	FORCEINLINE bool HasZero() const { return ( _X == 0 || _Y == 0 ); }

	FORCEINLINE static ScreenPoint ToScreenCoordinate(const ScreenPoint& InScreenSize, const Vector2& InPos)
	{
		return ScreenPoint(InPos._X + InScreenSize._X * 0.5f, -InPos._Y + InScreenSize._Y * 0.5f);
	}

	FORCEINLINE Vector2 ToVectorCoordinate(const ScreenPoint& InScreenSize)
	{
		return Vector2(_X - InScreenSize._X * 0.5f + 0.5f, -_Y + InScreenSize._Y * 0.5f + 0.5f);
	}

	FORCEINLINE ScreenPoint operator-(const ScreenPoint& InPoint) const;
	FORCEINLINE ScreenPoint operator+(const ScreenPoint& InPoint) const;

	int _X = 0;
	int _Y = 0;
};

FORCEINLINE ScreenPoint ScreenPoint::operator-(const ScreenPoint& InPoint) const
{
	return ScreenPoint(_X - InPoint._X, _Y - InPoint._Y);
}

FORCEINLINE ScreenPoint ScreenPoint::operator+(const ScreenPoint& InPoint) const
{
	return ScreenPoint(_X + InPoint._X, _Y + InPoint._Y);
}

