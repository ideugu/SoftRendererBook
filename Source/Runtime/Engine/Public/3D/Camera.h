#pragma once

namespace CK
{
namespace DDD
{

class Camera
{
public:
	Camera() { }
	~Camera() { }

public:
	Transform& GetTransform() { return _Transform; }
	Matrix4x4 GetViewMatrix() const;
	void SetLookAtRotation(const Vector3& InTargetPosition);

protected:
	Transform _Transform;
};

}
}