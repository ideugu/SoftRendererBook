
#include "Precompiled.h"
#include "Transform2D.h"

Matrix3x3 Transform2D::GetModelingMatrix() const
{
	Matrix3x3 translateMat(Vector3::UnitX, Vector3::UnitY, Vector3(Position));

	Matrix3x3 rotationMat(
		Vector3(Right, false),
		Vector3(Up, false),
		Vector3::UnitZ);

	Matrix3x3 scaleMat(Vector3::UnitX * Scale.X, Vector3::UnitY * Scale.Y, Vector3::UnitZ);
	return translateMat * rotationMat * scaleMat;
}

void Transform2D::CalculateLocalAxis()
{
	float sin, cos;
	Math::GetSinCos(sin, cos, Rotation);

	Right = Vector2(cos, sin);
	Up = Vector2(-sin, cos);
}
