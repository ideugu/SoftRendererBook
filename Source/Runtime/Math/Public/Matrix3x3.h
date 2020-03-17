#pragma once

#include "Vector3.h"

struct Matrix3x3
{
public:
	// 생성자 
	FORCEINLINE Matrix3x3();
	FORCEINLINE explicit Matrix3x3(const Vector3& InCol0, const Vector3& InCol1, const Vector3& InCol2);
	FORCEINLINE explicit Matrix3x3(float In00, float In01, float In02, float In10, float In11, float In12, float In20, float In21, float In22);

	// 연산자 
	FORCEINLINE const Vector3& operator[](int InIndex) const;
	FORCEINLINE Vector3& operator[](int InIndex);

	FORCEINLINE Matrix3x3 operator*(float InScalar) const;
	FORCEINLINE Matrix3x3 operator*(const Matrix3x3& InMatrix) const;
	FORCEINLINE Vector3 operator*(const Vector3& InVector) const;
	FORCEINLINE friend Vector3 operator*=(Vector3& InVector, const Matrix3x3& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}
	FORCEINLINE Vector2 operator*(const Vector2& InVector) const;
	FORCEINLINE friend Vector2 operator*=(Vector2& InVector, const Matrix3x3& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}

	// 멤버함수 
	FORCEINLINE void SetIdentity();
	FORCEINLINE Matrix3x3 Tranpose() const;

	// 정적멤버변수 
	static const Matrix3x3 Identity;

	// 멤버변수 
	Vector3 _Cols[3];
};

FORCEINLINE Matrix3x3::Matrix3x3()
{
	*this = Matrix3x3::Identity;
}

FORCEINLINE Matrix3x3::Matrix3x3(const Vector3& InCol0, const Vector3& InCol1, const Vector3& InCol2)
{
	_Cols[0] = InCol0;
	_Cols[1] = InCol1;
	_Cols[2] = InCol2;
}

FORCEINLINE Matrix3x3::Matrix3x3(float In00, float In01, float In02, float In10, float In11, float In12, float In20, float In21, float In22)
{
	_Cols[0][0] = In00;
	_Cols[0][1] = In01;
	_Cols[0][2] = In02;

	_Cols[1][0] = In10;
	_Cols[1][1] = In11;
	_Cols[1][2] = In12;

	_Cols[2][0] = In20;
	_Cols[2][1] = In21;
	_Cols[2][2] = In22;
}

FORCEINLINE void Matrix3x3::SetIdentity()
{
	_Cols[0] = Vector3::UnitX;
	_Cols[1] = Vector3::UnitY;
	_Cols[2] = Vector3::UnitZ;
}

FORCEINLINE Matrix3x3 Matrix3x3::Tranpose() const
{
	return Matrix3x3(
		Vector3(_Cols[0]._X, _Cols[1]._X, _Cols[2]._X),
		Vector3(_Cols[0]._Y, _Cols[1]._Y, _Cols[2]._Y),
		Vector3(_Cols[0]._Z, _Cols[1]._Z, _Cols[2]._Z)
	);
}

FORCEINLINE const Vector3& Matrix3x3::operator[](int InIndex) const
{
	assert(InIndex >= 0 && InIndex <= 2);
	return _Cols[InIndex];
}

FORCEINLINE Vector3& Matrix3x3::operator[](int InIndex)
{
	assert(InIndex >= 0 && InIndex <= 2);
	return _Cols[InIndex];
}

FORCEINLINE Matrix3x3 Matrix3x3::operator*(float InScalar) const
{
	return Matrix3x3(
		_Cols[0] * InScalar,
		_Cols[1] * InScalar,
		_Cols[2] * InScalar
	);
}

FORCEINLINE Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &InMatrix) const
{
	Matrix3x3 tpMat = Tranpose();
	return Matrix3x3(
		Vector3(tpMat[0].Dot(InMatrix[0]), tpMat[1].Dot(InMatrix[0]), tpMat[2].Dot(InMatrix[0])),
		Vector3(tpMat[0].Dot(InMatrix[1]), tpMat[1].Dot(InMatrix[1]), tpMat[2].Dot(InMatrix[1])),
		Vector3(tpMat[0].Dot(InMatrix[2]), tpMat[1].Dot(InMatrix[2]), tpMat[2].Dot(InMatrix[2]))
	);

}

FORCEINLINE Vector3 Matrix3x3::operator*(const Vector3& InVector) const
{
	Matrix3x3 tpMat = Tranpose();
	return Vector3(
		tpMat[0].Dot(InVector),
		tpMat[1].Dot(InVector),
		tpMat[2].Dot(InVector)
	);
}

FORCEINLINE Vector2 Matrix3x3::operator*(const Vector2& InVector) const
{
	Vector3 v3(InVector);
	v3 *= *this;

	return v3.ToVector2();
}
