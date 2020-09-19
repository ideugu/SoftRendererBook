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
	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;

private:
	Transform _Transform;
};

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	const Vector3& localX = _Transform.GetLocalX();
	const Vector3& localY = _Transform.GetLocalY();
	const Vector3& localZ = _Transform.GetLocalZ();
	Vector3 pos = _Transform.GetPosition();
	return Matrix4x4(
		Vector4(localX, false),
		Vector4(localY, false),
		Vector4(localZ, false),
		Vector4(-localX.Dot(pos), -localY.Dot(pos), -localZ.Dot(pos), 1.f)
	);
}

FORCEINLINE Matrix4x4 Camera::GetViewMatrixRotationOnly() const
{
	const Vector3& localX = _Transform.GetLocalX();
	const Vector3& localY = _Transform.GetLocalY();
	const Vector3& localZ = _Transform.GetLocalZ();
	return Matrix4x4(
		Vector4(localX, false),
		Vector4(localY, false),
		Vector4(localZ, false),
		Vector4::UnitW
	);
}

}
}