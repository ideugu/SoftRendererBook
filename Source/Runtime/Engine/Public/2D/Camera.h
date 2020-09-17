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
	Matrix3x3 GetViewMatrix() const;

protected:
	Transform _Transform;
};

}
}