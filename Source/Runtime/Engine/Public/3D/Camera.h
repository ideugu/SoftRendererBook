#pragma once

namespace CK
{
namespace DDD
{

class Camera
{
public:
	Camera() = default;
	~Camera() { }

public:
	Transform& GetTransform() { return _Transform; }
	void SetLookAtRotation(const Vector3& InTargetPosition);

	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;

private:
	Transform _Transform;
};

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	Vector3 viewX, viewY;
	Vector3 viewZ = -_Transform.GetLocalZ();
	if (viewZ.EqualsInTolerance(Vector3::UnitY) || viewZ.EqualsInTolerance(-Vector3::UnitY))
	{
		viewX = Vector3::UnitZ;
	}
	else
	{
		viewX = Vector3::UnitY.Cross(viewZ).Normalize();
	}
	viewY = viewZ.Cross(viewX);
	Vector3 pos = _Transform.GetPosition();
	return Matrix4x4(
		Vector4(Vector3(viewX.X, viewY.X, viewZ.X), false),
		Vector4(Vector3(viewX.Y, viewY.Y, viewZ.Y), false),
		Vector4(Vector3(viewX.Z, viewY.Z, viewZ.Z), false),
		Vector4(-viewX.Dot(pos), -viewY.Dot(pos), -viewZ.Dot(pos), 1.f)
	);
}

FORCEINLINE Matrix4x4 Camera::GetViewMatrixRotationOnly() const
{
	Vector3 viewX, viewY;
	Vector3 viewZ = -_Transform.GetLocalZ();
	if (viewZ.EqualsInTolerance(Vector3::UnitY) || viewZ.EqualsInTolerance(-Vector3::UnitY))
	{
		viewX = Vector3::UnitZ;
	}
	else
	{
		viewX = Vector3::UnitY.Cross(viewZ).Normalize();
	}
	viewY = viewZ.Cross(viewX);
	return Matrix4x4(
		Vector4(Vector3(viewX.X, viewY.X, viewZ.X), false),
		Vector4(Vector3(viewX.Y, viewY.Y, viewZ.Y), false),
		Vector4(Vector3(viewX.Z, viewY.Z, viewZ.Z), false),
		Vector4::UnitW
	);
}

}
}