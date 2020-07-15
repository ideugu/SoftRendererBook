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
	Matrix4x4 GetViewMatrix() const;
	Matrix4x4 GetPerspectiveMatrix(int InScreenSizeX, int InScreenSizeY) const;
	void SetLookAtRotation(const Vector3& InTargetPosition);

private:
	Transform _Transform;

	float _FOV = 60.f;
	float _NearZ = 5.5f;
	float _FarZ = 1000.f;
};

}
}