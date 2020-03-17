#pragma once

#include "Vector4.h"
#include "Matrix2x2.h"
#include "Matrix3x3.h"

struct Matrix4x4
{
public:
	// 생성자 
	FORCEINLINE Matrix4x4();
	FORCEINLINE explicit Matrix4x4(const Vector4& InCol0, const Vector4& InCol1, const Vector4& InCol2, const Vector4& InCol3);

	// 연산자 
	FORCEINLINE const Vector4& operator[](int InIndex) const;
	FORCEINLINE Vector4& operator[](int InIndex);
	FORCEINLINE Matrix4x4 operator*(float InScalar) const;
	FORCEINLINE Matrix4x4 operator*(const Matrix4x4& InMatrix) const;
	FORCEINLINE Vector4 operator*(const Vector4& InVector) const;
	FORCEINLINE friend Vector4 operator*=(Vector4& InVector, const Matrix4x4& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}
	FORCEINLINE Vector3 operator*(const Vector3& InVector) const;
	FORCEINLINE friend Vector3 operator*=(Vector3& InVector, const Matrix4x4& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}

	// 멤버함수 
	FORCEINLINE void SetIdentity();
	FORCEINLINE Matrix4x4 Tranpose() const;

	// 정적멤버변수 
	static const Matrix4x4 Identity;

	// 멤버변수 
	Vector4 _Cols[4];
};

FORCEINLINE Matrix4x4::Matrix4x4()
{
	*this = Matrix4x4::Identity;
}

FORCEINLINE Matrix4x4::Matrix4x4(const Vector4& InCol0, const Vector4& InCol1, const Vector4& InCol2, const Vector4& InCol3)
{
	_Cols[0] = InCol0;
	_Cols[1] = InCol1;
	_Cols[2] = InCol2;
	_Cols[3] = InCol3;
}

FORCEINLINE void Matrix4x4::SetIdentity()
{
	_Cols[0] = Vector4::UnitX;
	_Cols[1] = Vector4::UnitY;
	_Cols[2] = Vector4::UnitZ;
	_Cols[3] = Vector4::UnitW;
}

FORCEINLINE Matrix4x4 Matrix4x4::Tranpose() const
{
	return Matrix4x4(
		Vector4(_Cols[0]._X, _Cols[1]._X, _Cols[2]._X, _Cols[3]._X),
		Vector4(_Cols[0]._Y, _Cols[1]._Y, _Cols[2]._Y, _Cols[3]._Y),
		Vector4(_Cols[0]._Z, _Cols[1]._Z, _Cols[2]._Z, _Cols[3]._Z),
		Vector4(_Cols[0]._W, _Cols[1]._W, _Cols[2]._W, _Cols[3]._W)
	);
}

FORCEINLINE const Vector4& Matrix4x4::operator[](int InIndex) const
{
	assert(InIndex >= 0 && InIndex <= 3);
	return _Cols[InIndex];
}

FORCEINLINE Vector4& Matrix4x4::operator[](int InIndex)
{
	assert(InIndex >= 0 && InIndex <= 3);
	return _Cols[InIndex];
}

FORCEINLINE Matrix4x4 Matrix4x4::operator*(float InScalar) const
{
	return Matrix4x4(
		_Cols[0] * InScalar,
		_Cols[1] * InScalar,
		_Cols[2] * InScalar,
		_Cols[3] * InScalar
	);
}

FORCEINLINE Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &InMatrix) const
{
	Matrix4x4 tpMat = Tranpose();
	return Matrix4x4(
		Vector4(tpMat[0].Dot(InMatrix[0]), tpMat[1].Dot(InMatrix[0]), tpMat[2].Dot(InMatrix[0]), tpMat[3].Dot(InMatrix[0])),
		Vector4(tpMat[0].Dot(InMatrix[1]), tpMat[1].Dot(InMatrix[1]), tpMat[2].Dot(InMatrix[1]), tpMat[3].Dot(InMatrix[1])),
		Vector4(tpMat[0].Dot(InMatrix[2]), tpMat[1].Dot(InMatrix[2]), tpMat[2].Dot(InMatrix[2]), tpMat[3].Dot(InMatrix[2])),
		Vector4(tpMat[0].Dot(InMatrix[3]), tpMat[1].Dot(InMatrix[3]), tpMat[2].Dot(InMatrix[3]), tpMat[3].Dot(InMatrix[3]))
	);
}

FORCEINLINE Vector4 Matrix4x4::operator*(const Vector4& InVector) const
{
	Matrix4x4 tpMat = Tranpose();
	return Vector4(
		tpMat[0].Dot(InVector),
		tpMat[1].Dot(InVector),
		tpMat[2].Dot(InVector),
		tpMat[3].Dot(InVector)
	);
}

FORCEINLINE Vector3 Matrix4x4::operator*(const Vector3& InVector) const
{
	Vector4 v4(InVector);
	v4 *= *this;

	return v4.ToVector3();
}
