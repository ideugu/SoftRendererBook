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
	// 트랜스폼
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransformConst() const { return _Transform; }

	// 카메라 값을 가져오는 함수
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// 카메라 값을 설정하는 함수
	void SetViewportSize(const ScreenPoint& InViewportSize) { _ViewportSize = InViewportSize; }

	// 행렬 생성
	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;

private:
	Transform _Transform;

	ScreenPoint _ViewportSize;
};

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	const Vector3& localX = _Transform.GetLocalX();
	const Vector3& localY = _Transform.GetLocalY();
	const Vector3& localZ = _Transform.GetLocalZ();
	Vector3 pos = _Transform.GetPosition();
	return Matrix4x4(
		Vector4(Vector3(localX.X, localY.X, localZ.X), false),
		Vector4(Vector3(localX.Y, localY.Y, localZ.Y), false),
		Vector4(Vector3(localX.Z, localY.Z, localZ.Z), false),
		Vector4(-localX.Dot(pos), -localY.Dot(pos), -localZ.Dot(pos), 1.f)
	);
}

FORCEINLINE Matrix4x4 Camera::GetViewMatrixRotationOnly() const
{
	const Vector3& localX = _Transform.GetLocalX();
	const Vector3& localY = _Transform.GetLocalY();
	const Vector3& localZ = _Transform.GetLocalZ();
	return Matrix4x4(
		Vector4(Vector3(localX.X, localY.X, localZ.X), false),
		Vector4(Vector3(localX.Y, localY.Y, localZ.Y), false),
		Vector4(Vector3(localX.Z, localY.Z, localZ.Z), false),
		Vector4::UnitW
	);
}

}
}