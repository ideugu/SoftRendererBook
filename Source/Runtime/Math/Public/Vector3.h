#pragma once

#include "Vector2.h"

struct Vector3
{
public:
	// 생성자 
	FORCEINLINE Vector3() = default;
	FORCEINLINE explicit Vector3(const Vector2& InV, bool IsPoint = true) : _X(InV._X), _Y(InV._Y) { _Z = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit Vector3(float InX, float InY, float InZ) : _X(InX), _Y(InY), _Z(InZ) { }

	// 연산자 
	FORCEINLINE float operator[](int InIndex) const;
	FORCEINLINE float& operator[](int InIndex);
	FORCEINLINE Vector3 operator-() const;
	FORCEINLINE Vector3 operator*(float InScale) const;
	FORCEINLINE Vector3 operator/(float InScale) const;
	FORCEINLINE Vector3 operator+(const Vector3& InVector) const;
	FORCEINLINE Vector3 operator-(const Vector3& InVector) const;
	FORCEINLINE Vector3& operator*=(float InScale);
	FORCEINLINE Vector3& operator/=(float InScale);
	FORCEINLINE Vector3& operator+=(const Vector3& InVector);
	FORCEINLINE Vector3& operator-=(const Vector3& InVector);

	// 멤버함수 
	FORCEINLINE Vector2 ToVector2() const;
	FORCEINLINE float Size() const;
	FORCEINLINE float SizeSquared() const;
	FORCEINLINE Vector3 Normalize() const;
	FORCEINLINE bool EqualsInTolerance(const Vector3& InVector, float InTolerance = KINDA_SMALL_NUMBER) const;
	FORCEINLINE float Max() const;
	FORCEINLINE float Dot(const Vector3& InVector) const;
	FORCEINLINE Vector3 Cross(const Vector3& InVector) const;

	// 정적멤버변수 
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 One;
	static const Vector3 Zero;
	static const Vector3 Infinity;
	static const Vector3 InfinityNeg;

	// 멤버변수 
	float _X = 0.f;
	float _Y = 0.f;
	float _Z = 0.f;
};

FORCEINLINE Vector2 Vector3::ToVector2() const
{
	return Vector2(_X, _Y);
}

FORCEINLINE float Vector3::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE float Vector3::SizeSquared() const
{
	return _X * _X + _Y * _Y + _Z * _Z;
}

FORCEINLINE Vector3 Vector3::Normalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if(squareSum == 0.f)
	{
		return Vector3::Zero;
	}

	float invLength = 1 / sqrtf(squareSum);
	return Vector3(_X * invLength, _Y * invLength, _Z * invLength);
}

FORCEINLINE float Vector3::operator[](int InIndex) const
{
	assert(InIndex >= 0 && InIndex <= 2);
	return ((float *)this)[InIndex];
}

FORCEINLINE float &Vector3::operator[](int InIndex)
{
	assert(InIndex >= 0 && InIndex <= 2);
	return ((float *)this)[InIndex];
}

FORCEINLINE Vector3 Vector3::operator-() const
{
	return Vector3(-_X, -_Y, -_Z);
}

FORCEINLINE Vector3 Vector3::operator*(float InScale) const
{
	return Vector3(_X * InScale, _Y * InScale, _Z * InScale);
}

FORCEINLINE Vector3 Vector3::operator/(float InScale) const
{
	return Vector3(_X / InScale, _Y / InScale, _Z / InScale);
}

FORCEINLINE Vector3 Vector3::operator+(const Vector3& InVector) const
{
	return Vector3(_X + InVector._X, _Y + InVector._Y, _Z + InVector._Z);
}

FORCEINLINE Vector3 Vector3::operator-(const Vector3& InVector) const
{
	return Vector3(_X - InVector._X, _Y - InVector._Y, _Z - InVector._Z);
}

FORCEINLINE Vector3& Vector3::operator*=(float InScale)
{
	_X *= InScale;
	_Y *= InScale;
	_Z *= InScale;
	return *this;
}

FORCEINLINE Vector3& Vector3::operator/=(float InScale)
{
	_X /= InScale;
	_Y /= InScale;
	_Z /= InScale;
	return *this;
}

FORCEINLINE Vector3& Vector3::operator+=(const Vector3& InVector)
{
	_X += InVector._X;
	_Y += InVector._Y;
	_Z += InVector._Z;
	return *this;
}

FORCEINLINE Vector3& Vector3::operator-=(const Vector3& InVector)
{
	_X -= InVector._X;
	_Y -= InVector._Y;
	_Z -= InVector._Z;
	return *this;
}

FORCEINLINE bool Vector3::EqualsInTolerance(const Vector3& InVector, float InTolerance) const
{
	return (Math::Abs(this->_X - InVector._X) <= InTolerance) &&
		(Math::Abs(this->_Y - InVector._Y) < InTolerance) &&
		(Math::Abs(this->_Z - InVector._Z) < InTolerance);
}

FORCEINLINE float Vector3::Max() const
{
	float max = Math::Max(_X, _Y);
	return Math::Max(max, _Z);
}

FORCEINLINE float Vector3::Dot(const Vector3& InVector) const
{
	return _X * InVector._X + _Y * InVector._Y + _Z * InVector._Z;
}

FORCEINLINE Vector3 Vector3::Cross(const Vector3& InVector) const
{
	return Vector3(
		_Y * InVector._Z - _Z * InVector._Y,
		_Z * InVector._X - _X * InVector._Z,
		_X * InVector._Y - _Y * InVector._X);
}
