#pragma once

namespace CK
{
namespace DD
{

class Camera
{
public:
	Camera() { }
	~Camera() { }

public:
	Transform& GetTransform() { return _Transform; }
	Matrix3x3 GetViewMatrix() const;
	void SetCameraViewSize(const ScreenPoint& InScreenSize);
	void SetCameraCircleBound(float InRadius) { CircleBound.Radius = InRadius; }
	const Circle& GetCircleBound() const { return CircleBound; }

protected:
	Transform _Transform;

	Circle CircleBound;
};

}
}