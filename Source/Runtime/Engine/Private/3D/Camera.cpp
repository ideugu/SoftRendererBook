
#include "Precompiled.h"
using namespace CK::DDD;

Matrix4x4 Camera::GetViewMatrix() const
{
	Matrix4x4 inverseRotation = Matrix4x4(Vector4(_Transform._Right, false), Vector4(_Transform._Up, false), Vector4(_Transform._Forward, false), Vector4::UnitW).Tranpose();
	Matrix4x4 inverseTranslate = Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-_Transform._Position, true));
	return inverseRotation * inverseTranslate;
}

Matrix4x4 Camera::GetPerspectiveMatrix(int InScreenSizeX, int InScreenSizeY) const
{
	// 투영 행렬. 깊이 값의 범위는 -1~1
	float invA = (float)InScreenSizeY / (float)InScreenSizeX;

	float focalLength = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);
	float invNF = 1.f / (_NearZ - _FarZ);
	float k = (_FarZ + _NearZ) * invNF;
	float l = 2.f * _FarZ * _NearZ * invNF;
	return Matrix4x4(
		Vector4::UnitX * invA * focalLength,
		Vector4::UnitY * focalLength,
		Vector4(0.f, 0.f, k, -1.f),
		Vector4(0.f, 0.f, l, 0.f));

}

void Camera::SetLookAtRotation(const Vector3& InTargetPosition)
{
	_Transform._Forward = (_Transform._Position - InTargetPosition).Normalize();
	if (_Transform._Forward.EqualsInTolerance(Vector3::UnitY) || _Transform._Forward.EqualsInTolerance(-Vector3::UnitY))
	{
		_Transform._Right = Vector3::UnitX;
	}
	else
	{
		_Transform._Right = Vector3::UnitY.Cross(_Transform._Forward).Normalize();
	}
	_Transform._Up = _Transform._Forward.Cross(_Transform._Right).Normalize();
}
