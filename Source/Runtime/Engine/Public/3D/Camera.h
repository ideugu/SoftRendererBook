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
	TransformNode& GetTransformNode() { return _TransformNode; }
	const TransformNode& GetTransformNode() const { return _TransformNode; }

	// 카메라 값을 가져오는 함수
	float GetFOV() const { return _FOV; }
	float GetNearZ() const { return _NearZ; }
	float GetFarZ() const { return _FarZ; }
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// 카메라 값을 설정하는 함수
	void SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp = Vector3::UnitY);
	void SetFOV(float InFOV) { _FOV = InFOV; }
	void SetNearZ(float InNearZ) { _NearZ = InNearZ; }
	void SetFarZ(float InFarZ) { _FarZ = InFarZ; }
	void SetViewportSize(const ScreenPoint& InViewportSize) { _ViewportSize = InViewportSize; }

	// 행렬 생성
	FORCEINLINE void GetViewLocalAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const;
	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;
	FORCEINLINE Matrix4x4 GetPerspectiveMatrix() const;

private:
	TransformNode _TransformNode;

	float _FOV = 60.f;
	float _NearZ = 5.5f;
	float _FarZ = 3000.f;
	ScreenPoint _ViewportSize;
};

FORCEINLINE void Camera::GetViewLocalAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const
{
	OutViewZ = -_TransformNode.GetLocalZ();
	OutViewX = -_TransformNode.GetLocalX();
	OutViewY = _TransformNode.GetLocalY();
}

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	Vector3 viewX, viewY, viewZ;
	GetViewLocalAxes(viewX, viewY, viewZ);
	Vector3 pos = _TransformNode.GetWorldPosition();

	return Matrix4x4(
		Vector4(Vector3(viewX.X, viewY.X, viewZ.X), false),
		Vector4(Vector3(viewX.Y, viewY.Y, viewZ.Y), false),
		Vector4(Vector3(viewX.Z, viewY.Z, viewZ.Z), false),
		Vector4(-viewX.Dot(pos), -viewY.Dot(pos), -viewZ.Dot(pos), 1.f)
	);
}

FORCEINLINE Matrix4x4 Camera::GetViewMatrixRotationOnly() const
{
	Vector3 viewX, viewY, viewZ;
	GetViewLocalAxes(viewX, viewY, viewZ);

	return Matrix4x4(
		Vector4(Vector3(viewX.X, viewY.X, viewZ.X), false),
		Vector4(Vector3(viewX.Y, viewY.Y, viewZ.Y), false),
		Vector4(Vector3(viewX.Z, viewY.Z, viewZ.Z), false),
		Vector4::UnitW
	);
}

FORCEINLINE Matrix4x4 Camera::GetPerspectiveMatrix() const
{
	// 투영 행렬. 깊이 값의 범위는 -1~1
	float invA = 1.f / _ViewportSize.AspectRatio();
	float d = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);

	// 근평면과 원평면에 반대 부호를 붙여서 계산
	float invNF = 1.f / (_NearZ - _FarZ);
	float k = (_FarZ + _NearZ) * invNF;
	float l = 2.f * _FarZ * _NearZ * invNF;
	return Matrix4x4(
		Vector4::UnitX * invA * d,
		Vector4::UnitY * d,
		Vector4(0.f, 0.f, k, -1.f),
		Vector4(0.f, 0.f, l, 0.f));
}

}
}