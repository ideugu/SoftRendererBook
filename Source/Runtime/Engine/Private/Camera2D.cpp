
#include "Precompiled.h"

Matrix3x3 Camera2D::GetViewMatrix() const
{
	return Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(-_Transform.GetPosition()));
}

void Camera2D::SetCameraViewSize(const ScreenPoint& InScreenSize)
{
	Vector2 halfSize = Vector2(InScreenSize.X, InScreenSize.Y) * 0.5f;

	CircleBounds.Center = Vector2::Zero;
	CircleBounds.Radius = halfSize.Size();
}