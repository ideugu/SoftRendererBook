
#include "Precompiled.h"
using namespace CK::DDD;

void Camera::SetLookAtRotation(const Vector3& InTargetPosition)
{
	Vector3 r, u, f;
	f = (_Transform.GetPosition() - InTargetPosition).Normalize();
	if (f.EqualsInTolerance(Vector3::UnitY) || f.EqualsInTolerance(-Vector3::UnitY))
	{
		r = Vector3::UnitX;
	}
	else
	{
		r = Vector3::UnitY.Cross(f).Normalize();
	}
	u = f.Cross(r).Normalize();

	_Transform.SetLocalAxes(r, u, f);
}
