
#include "Precompiled.h"
using namespace CK::DDD;

Matrix4x4 Camera::GetViewMatrix() const
{
	Matrix4x4 inverseRotation = Matrix4x4(Vector4(_Transform._Right, false), Vector4(_Transform._Up, false), Vector4(_Transform._Forward, false), Vector4::UnitW).Tranpose();
	Matrix4x4 inverseTranslate = Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-_Transform._Position, true));
	return inverseRotation * inverseTranslate;
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
