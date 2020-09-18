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

private:
	Transform _Transform;

	float _FOV = 60.f;
	float _NearZ = 5.5f;
	float _FarZ = 1000.f;
};

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	Matrix4x4 inverseRotation = Matrix4x4(Vector4(_Transform.GetLocalX(), false), Vector4(_Transform.GetLocalY(), false), Vector4(_Transform.GetLocalZ(), false), Vector4::UnitW).Tranpose();
	Matrix4x4 inverseTranslate = Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-_Transform.GetPosition()));
	return inverseRotation * inverseTranslate;
}

}
}