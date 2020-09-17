#pragma once

namespace CK
{
namespace DD
{

class Camera
{
public:
	Camera() = default;

public:
	Transform& GetTransform() { return _Transform; }
	FORCEINLINE Matrix3x3 GetViewMatrix() const;

protected:
	Transform _Transform;
};

FORCEINLINE Matrix3x3 Camera::GetViewMatrix() const
{
	return Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(-_Transform.GetPosition()));
}

}
}