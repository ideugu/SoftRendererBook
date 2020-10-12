#pragma once

namespace CK
{
namespace DD
{

class Camera
{
public:
	Camera() = default;
	~Camera() { }

public:
	// 트랜스폼
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransform() const { return _Transform; }

	// 카메라 값을 가져오는 함수
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// 카메라 값을 설정하는 함수
	void SetViewportSize(const ScreenPoint & InViewportSize) { _ViewportSize = InViewportSize; }

	// 행렬 생성
	FORCEINLINE Matrix3x3 GetViewMatrix() const;

private:
	Transform _Transform;
	ScreenPoint _ViewportSize;
};

FORCEINLINE Matrix3x3 Camera::GetViewMatrix() const
{
	return Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(-_Transform.GetPosition()));
}

}
}