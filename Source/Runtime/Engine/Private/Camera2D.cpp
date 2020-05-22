
#include "Precompiled.h"

Matrix3x3 Camera2D::GetViewMatrix() const
{
	return Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(-_Transform.GetPosition()));
}
