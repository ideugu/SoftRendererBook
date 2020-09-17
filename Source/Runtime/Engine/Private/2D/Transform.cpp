
#include "Precompiled.h"
using namespace CK::DD;

Matrix3x3 Transform::GetModelingMatrix() const
{
	return Matrix3x3(
		Vector3(Scale.X * Right.X, Scale.X * Right.Y, 0.f),
		Vector3(Scale.Y * Up.X, Scale.Y * Up.Y, 0.f),
		Vector3(Position.X, Position.Y, 1.f)
	);
}

void Transform::CalculateLocalAxis()
{
	float sin, cos;
	Math::GetSinCos(sin, cos, Rotation);

	Right = Vector2(cos, sin);
	Up = Vector2(-sin, cos);
}
