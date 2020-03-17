#pragma once

#include "Vector3.h"

struct Vector4
{
public:
	// 생성자 
	FORCEINLINE Vector4() = default;
	FORCEINLINE explicit Vector4(const Vector2& InV, bool IsPoint = true) : _X(InV._X), _Y(InV._Y), _Z(0.f) { _W = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit Vector4(const Vector3& InV, bool IsPoint = true) : _X(InV._X), _Y(InV._Y), _Z(InV._Z) { _W = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit Vector4(float InX, float InY, float InZ, float InW) : _X(InX), _Y(InY), _Z(InZ), _W(InW) { }
	FORCEINLINE explicit Vector4(float InX, float InY, float InZ, bool IsPoint = true) : _X(InX), _Y(InY), _Z(InZ) { _W = IsPoint ? 1.f : 0.f; }

	// 연산자 
	FORCEINLINE float operator[](int InIndex) const;
	FORCEINLINE float& operator[](int InIndex);
	FORCEINLINE Vector4 operator-() const;
	FORCEINLINE Vector4 operator*(float InScale) const;
	FORCEINLINE Vector4 operator/(float InScale) const;
	FORCEINLINE Vector4 operator+(const Vector4& InVector) const;
	FORCEINLINE Vector4 operator-(const Vector4& InVector) const;
	FORCEINLINE Vector4& operator*=(float InScale);
	FORCEINLINE Vector4& operator/=(float InScale);
	FORCEINLINE Vector4& operator+=(const Vector4& InVector);
	FORCEINLINE Vector4& operator-=(const Vector4& InVector);

	// 멤버함수 
	FORCEINLINE Vector2 ToVector2() const;
	FORCEINLINE Vector3 ToVector3() const;
	FORCEINLINE float Size() const;
	FORCEINLINE float SizeSquared() const;
	FORCEINLINE Vector4 Normalize() const;
	FORCEINLINE bool EqualsInTolerance(const Vector4& InVector, float InTolerance = KINDA_SMALL_NUMBER) const;
	FORCEINLINE float Max() const;
	FORCEINLINE float Dot(const Vector4& InVector) const;

	// 정적멤버변수 
	static const Vector4 UnitX;
	static const Vector4 UnitY;
	static const Vector4 UnitZ;
	static const Vector4 UnitW;
	static const Vector4 One;
	static const Vector4 Zero;
	static const Vector4 Infinity;
	static const Vector4 InfinityNeg;

	// 멤버변수 
	float _X = 0.f;
	float _Y = 0.f;
	float _Z = 0.f;
	float _W = 0.f;
};

FORCEINLINE Vector2 Vector4::ToVector2() const
{
	return Vector2(_X, _Y);
}

FORCEINLINE Vector3 Vector4::ToVector3() const
{
	return Vector3(_X, _Y, _Z);
}

FORCEINLINE float Vector4::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE float Vector4::SizeSquared() const
{
	return _X * _X + _Y * _Y + _Z * _Z + _W * _W;
}

FORCEINLINE Vector4 Vector4::Normalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if (squareSum == 0.f)
	{
		return Vector4::Zero;
	}

	float invLength = 1 / sqrtf(squareSum);
	return Vector4(_X * invLength, _Y * invLength, _Z * invLength, _W * invLength);
}

FORCEINLINE float Vector4::operator[](int InIndex) const
{
	assert(InIndex >= 0 && InIndex <= 3);
	return ((float *)this)[InIndex];
}

FORCEINLINE float& Vector4::operator[](int InIndex)
{
	assert(InIndex >= 0 && InIndex <= 3);
	return ((float *)this)[InIndex];
}

FORCEINLINE Vector4 Vector4::operator-() const
{
	return Vector4(-_X, -_Y, -_Z, -_W);
}

FORCEINLINE Vector4 Vector4::operator*(float InScale) const
{
	return Vector4(_X * InScale, _Y * InScale, _Z * InScale, _W * InScale);
}

FORCEINLINE Vector4 Vector4::operator/(float InScale) const
{
	return Vector4(_X / InScale, _Y / InScale, _Z / InScale, _W / InScale);
}

FORCEINLINE Vector4 Vector4::operator+(const Vector4& InV) const
{
	return Vector4(_X + InV._X, _Y + InV._Y, _Z + InV._Z, _W + InV._W);
}

FORCEINLINE Vector4 Vector4::operator-(const Vector4& InV) const
{
	return Vector4(_X - InV._X, _Y - InV._Y, _Z - InV._Z, _W - InV._W);
}

FORCEINLINE Vector4& Vector4::operator*=(float InScale)
{
	_X *= InScale;
	_Y *= InScale;
	_Z *= InScale;
	_W *= InScale;
	return *this;
}

FORCEINLINE Vector4& Vector4::operator/=(float InScale)
{
	_X /= InScale;
	_Y /= InScale;
	_Z /= InScale;
	_W /= InScale;
	return *this;
}

FORCEINLINE Vector4& Vector4::operator+=(const Vector4& InV)
{
	_X += InV._X;
	_Y += InV._Y;
	_Z += InV._Z;
	_W += InV._W;
	return *this;
}

FORCEINLINE Vector4& Vector4::operator-=(const Vector4& InV)
{
	_X -= InV._X;
	_Y -= InV._Y;
	_Z -= InV._Z;
	_W -= InV._W;
	return *this;
}

FORCEINLINE bool Vector4::EqualsInTolerance(const Vector4& InVector, float InTolerance) const
{
	return (Math::Abs(this->_X - InVector._X) <= InTolerance) &&
		(Math::Abs(this->_Y - InVector._Y) < InTolerance) &&
		(Math::Abs(this->_Z - InVector._Z) < InTolerance) &&
		(Math::Abs(this->_W - InVector._W) < InTolerance);
}

FORCEINLINE float Vector4::Max() const
{
	float max = Math::Max(_X, _Y);
	max = Math::Max(max, _Z);
	return Math::Max(max, _W);
}


FORCEINLINE float Vector4::Dot(const Vector4& InV) const
{
	return _X * InV._X + _Y * InV._Y + _Z * InV._Z + _W * InV._W;
}
