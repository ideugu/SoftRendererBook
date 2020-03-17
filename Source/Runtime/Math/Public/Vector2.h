#pragma once

#include "Platform.h"

struct Vector2
{
public:
	FORCEINLINE Vector2() = default;
	FORCEINLINE explicit Vector2(int InX, int InY) : _X((float)InX), _Y((float)InY) { }
	FORCEINLINE explicit Vector2(float InX, float InY) : _X(InX), _Y(InY) { }

	FORCEINLINE float Size() const;
	FORCEINLINE float SizeSquared() const;
	FORCEINLINE Vector2 Normalize() const;

	FORCEINLINE float operator[](int InIndex) const;
	FORCEINLINE float& operator[](int InIndex);

	FORCEINLINE Vector2 operator-() const;
	FORCEINLINE Vector2 operator*(float InScalar) const;
	FORCEINLINE Vector2 operator/(float InScalar) const;
	FORCEINLINE Vector2 operator+(const Vector2& InVector) const;
	FORCEINLINE Vector2 operator-(const Vector2& InVector) const;
	FORCEINLINE Vector2& operator*=(float InScale);
	FORCEINLINE Vector2& operator/=(float InScale);
	FORCEINLINE Vector2& operator+=(const Vector2& InVector);
	FORCEINLINE Vector2& operator-=(const Vector2& InVector);

	FORCEINLINE bool EqualsInRange(const Vector2& InVector, float InTolerance = KINDA_SMALL_NUMBER) const;

	FORCEINLINE float Max() const;
	FORCEINLINE float Dot(const Vector2& InVector) const;

	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 One;
	static const Vector2 Zero;
	static const Vector2 Infinity;
	static const Vector2 InfinityNeg;

public:
	float _X = 0.f;
	float _Y = 0.f;
};

FORCEINLINE float Vector2::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE float Vector2::SizeSquared() const
{
	return _X * _X + _Y * _Y;
}

FORCEINLINE Vector2 Vector2::Normalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if (squareSum == 0.f)
	{
		return Vector2::Zero;
	}

	float invLength = 1 / sqrtf(squareSum);
	return Vector2(_X * invLength, _Y * invLength);
}

FORCEINLINE float Vector2::operator[](int InIndex) const
{
	return ((float *)this)[InIndex];
}

FORCEINLINE float &Vector2::operator[](int InIndex)
{
	return ((float *)this)[InIndex];
}

FORCEINLINE Vector2 Vector2::operator-() const
{
	return Vector2(-_X, -_Y);
}

FORCEINLINE Vector2 Vector2::operator*(float InScalar) const
{
	return Vector2(_X * InScalar, _Y * InScalar);
}

FORCEINLINE Vector2 Vector2::operator/(float InScalar) const
{
	return Vector2(_X / InScalar, _Y / InScalar);
}

FORCEINLINE Vector2 Vector2::operator+(const Vector2& InVector) const
{
	return Vector2(_X + InVector._X, _Y + InVector._Y);
}

FORCEINLINE Vector2 Vector2::operator-(const Vector2& InVector) const
{
	return Vector2(_X - InVector._X, _Y - InVector._Y);
}

FORCEINLINE Vector2& Vector2::operator*=(float InScale)
{
	_X *= InScale;
	_Y *= InScale;
	return *this;
}

FORCEINLINE Vector2& Vector2::operator/=(float InScale)
{
	_X /= InScale;
	_Y /= InScale;
	return *this;
}

FORCEINLINE Vector2& Vector2::operator+=(const Vector2& InVector)
{
	_X += InVector._X;
	_Y += InVector._Y;
	return *this;
}

FORCEINLINE Vector2& Vector2::operator-=(const Vector2& InVector)
{
	_X -= InVector._X;
	_Y -= InVector._Y;
	return *this;
}

FORCEINLINE bool Vector2::EqualsInRange(const Vector2& InVector, float InTolerance) const
{
	return (Math::Abs(this->_X - InVector._X) <= InTolerance) &&
		(Math::Abs(this->_Y - InVector._Y) < InTolerance);
}

FORCEINLINE float Vector2::Max() const
{
	return Math::Max(_X, _Y);
}

FORCEINLINE float Vector2::Dot(const Vector2& InVector) const
{
	return _X * InVector._X + _Y * InVector._Y;
}
