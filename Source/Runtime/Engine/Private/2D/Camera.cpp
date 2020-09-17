
#include "Precompiled.h"
using namespace CK::DD;

Matrix3x3 Camera::GetViewMatrix() const
{
	return Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(-_Transform.GetPosition()));
}
