#pragma once

#include "Platform.h"
#include "Vector2.h"

struct Rectangle
{
public:
	Rectangle() = default;
	Rectangle(const Rectangle& InBox) : Min(InBox.Min), Max(InBox.Max) { }
	Rectangle(const Vector2& InMinVector, const Vector2& InMaxVector) : Min(InMinVector), Max(InMaxVector) { }
	Rectangle(const Vector2* InVertices, size_t InCount);

	FORCEINLINE bool Intersect(const Rectangle& InBox) const;
	FORCEINLINE bool IsInside(const Rectangle& InBox) const;
	FORCEINLINE bool IsInside(const Vector2& InVector) const;

	FORCEINLINE Rectangle operator+=(const Vector2& InVector);
	FORCEINLINE Rectangle operator+=(const Rectangle& InBox);

	FORCEINLINE Vector2 GetSize() const;
	FORCEINLINE Vector2 GetExtent() const;
	FORCEINLINE void GetCenterAndExtent(Vector2& OutCenter, Vector2& OutExtent) const;

public:
	Vector2 Min;
	Vector2 Max;
};

FORCEINLINE bool Rectangle::Intersect(const Rectangle& InBox) const
{
	if ((Min.X > InBox.Max.X) || (InBox.Min.X > Max.X))
	{
		return false;
	}

	if ((Min.Y > InBox.Max.Y) || (InBox.Min.Y > Max.Y))
	{
		return false;
	}

	return true;
}

FORCEINLINE bool Rectangle::IsInside(const Rectangle& InBox) const
{
	return (IsInside(InBox.Min) && IsInside(InBox.Max));
}

FORCEINLINE bool Rectangle::IsInside(const Vector2& InVector) const
{
	return ((InVector.X >= Min.X) && (InVector.X <= Max.X) && (InVector.Y >= Min.Y) && (InVector.Y <= Max.Y));
}

FORCEINLINE Rectangle Rectangle::operator+=(const Vector2& InVector)
{
	Min.X = Math::Min(Min.X, InVector.X);
	Min.Y = Math::Min(Min.Y, InVector.Y);

	Max.X = Math::Max(Max.X, InVector.X);
	Max.Y = Math::Max(Max.Y, InVector.Y);
	return *this;
}

FORCEINLINE Rectangle Rectangle::operator+=(const Rectangle& InBox)
{
	Min.X = Math::Min(Min.X, InBox.Min.X);
	Min.Y = Math::Min(Min.Y, InBox.Min.Y);

	Max.X = Math::Max(Max.X, InBox.Max.X);
	Max.Y = Math::Max(Max.Y, InBox.Max.Y);
	return *this;
}

FORCEINLINE Vector2 Rectangle::GetSize() const
{
	return (Max - Min);
}

FORCEINLINE Vector2 Rectangle::GetExtent() const
{
	return GetSize() * 0.5f;
}

FORCEINLINE void Rectangle::GetCenterAndExtent(Vector2 & OutCenter, Vector2 & OutExtent) const
{
	OutExtent = GetExtent();
	OutCenter = Min + OutExtent;
}

