#pragma once

#include "Vector2.h"

struct Matrix2x2
{
public:
	// 생성자 
	FORCEINLINE Matrix2x2();
	FORCEINLINE explicit Matrix2x2(const Vector2& InCol0, const Vector2& InCol1);
	FORCEINLINE explicit Matrix2x2(float In00, float In01, float In10, float In11);

	// 연산자 
	FORCEINLINE const Vector2& operator[](int InIndex) const;
	FORCEINLINE Vector2& operator[](int InIndex);
	FORCEINLINE Matrix2x2 operator*(float InScalar) const;
	FORCEINLINE Matrix2x2 operator*(const Matrix2x2& InMatrix) const;
	FORCEINLINE Vector2 operator*(const Vector2& InVector) const;
	FORCEINLINE friend Vector2 operator*=(Vector2& InVector, const Matrix2x2& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}

	// 멤버함수 
	FORCEINLINE void SetIdentity();
	FORCEINLINE Matrix2x2 Tranpose() const;

	// 정적멤버변수 
	static const Matrix2x2 Identity;

	// 멤버변수 
	Vector2 _Cols[2];
};

FORCEINLINE Matrix2x2::Matrix2x2()
{
	*this = Matrix2x2::Identity;
}

FORCEINLINE Matrix2x2::Matrix2x2(const Vector2& InCol0, const Vector2& InCol1)
{
	_Cols[0] = InCol0;
	_Cols[1] = InCol1;
}

FORCEINLINE Matrix2x2::Matrix2x2(float In00, float In01, float In10, float In11)
{
	_Cols[0][0] = In00;
	_Cols[0][1] = In01;

	_Cols[1][0] = In10;
	_Cols[1][1] = In11;
}

FORCEINLINE void Matrix2x2::SetIdentity()
{
	_Cols[0] = Vector2::UnitX;
	_Cols[1] = Vector2::UnitY;
}

FORCEINLINE Matrix2x2 Matrix2x2::Tranpose() const
{
	return Matrix2x2(
		Vector2(_Cols[0]._X, _Cols[1]._X),
		Vector2(_Cols[0]._Y, _Cols[1]._Y)
	);
}

FORCEINLINE const Vector2& Matrix2x2::operator[](int InIndex) const
{
	assert(InIndex >= 0 && InIndex <= 1);
	return _Cols[InIndex];
}

FORCEINLINE Vector2& Matrix2x2::operator[](int InIndex)
{
	assert(InIndex >= 0 && InIndex <= 1);
	return _Cols[InIndex];
}

FORCEINLINE Matrix2x2 Matrix2x2::operator*(float InScalar) const
{
	return Matrix2x2(
		_Cols[0] * InScalar,
		_Cols[1] * InScalar
	);
}

FORCEINLINE Matrix2x2 Matrix2x2::operator*(const Matrix2x2 &InMatrix) const
{
	Matrix2x2 tpMat = Tranpose();
	return Matrix2x2(
		Vector2(tpMat[0].Dot(InMatrix[0]), tpMat[1].Dot(InMatrix[0])),
		Vector2(tpMat[0].Dot(InMatrix[1]), tpMat[1].Dot(InMatrix[1]))
	);
}

FORCEINLINE Vector2 Matrix2x2::operator*(const Vector2& InVector) const
{
	Matrix2x2 tpMat = Tranpose();
	return Vector2(
		tpMat[0].Dot(InVector),
		tpMat[1].Dot(InVector)
	);
}
