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
	const Transform& GetTransformConst() const { return _Transform; }
	void SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp = Vector3::UnitY);
	void SetFOV(float InFOV) { _FOV = InFOV; }
	void SetNearZ(float InNearZ) { _NearZ = InNearZ; }
	void SetFarZ(float InFarZ) { _FarZ = InFarZ; }
	void SetViewportSize(const ScreenPoint& InViewportSize) { _ViewportSize = InViewportSize; }
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	FORCEINLINE void GetViewLocalAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const;
	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;
	FORCEINLINE Matrix4x4 GetPerspectiveMatrix() const;

private:
	Transform _Transform;

	float _FOV = 60.f;
	float _NearZ = 5.5f;
	float _FarZ = 1000.f;
	ScreenPoint _ViewportSize;
};

FORCEINLINE void Camera::GetViewLocalAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const
{
	OutViewZ = -_Transform.GetLocalZ();
	OutViewX = -_Transform.GetLocalX();
	OutViewY = _Transform.GetLocalY();
}

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	Vector3 viewX, viewY, viewZ;
	GetViewLocalAxes(viewX, viewY, viewZ);
	Vector3 pos = _Transform.GetPosition();

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
	float invNF = 1.f / (_FarZ - _NearZ);
	float k = -(_FarZ + _NearZ) * invNF;
	float l = 2.f * _FarZ * _NearZ * invNF;
	return Matrix4x4(
		Vector4::UnitX * invA * d,
		Vector4::UnitY * d,
		Vector4(0.f, 0.f, k, -1.f),
		Vector4(0.f, 0.f, l, 0.f));
}

}
}