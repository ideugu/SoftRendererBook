
#include "Precompiled.h"
using namespace CK::DDD;

void Camera::SetLookAtRotation(const Vector3& InTargetPosition)
{
	Vector3 localX, localY, localZ;
	localZ = (InTargetPosition - _Transform.GetPosition()).Normalize();
	if (localZ.EqualsInTolerance(Vector3::UnitY) || localZ.EqualsInTolerance(-Vector3::UnitY))
	{
		localX = Vector3::UnitZ;
	}
	else
	{
		localX = Vector3::UnitY.Cross(localZ).Normalize();
	}
	localY = localZ.Cross(localX).Normalize();

	_Transform.SetLocalAxes(localX, localY, localZ);
}
