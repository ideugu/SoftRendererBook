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
	// Ʈ������
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransformConst() const { return _Transform; }

	// ī�޶� ���� �������� �Լ�
	float GetFOV() const { return _FOV; }
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// ī�޶� ���� �����ϴ� �Լ�
	void SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp = Vector3::UnitY);
	void SetFOV(float InFOV) { _FOV = InFOV; }
	void SetViewportSize(const ScreenPoint& InViewportSize) { _ViewportSize = InViewportSize; }

	// ��� ����
	FORCEINLINE void GetViewLocalAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const;
	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;
	FORCEINLINE Matrix4x4 GetPerspectiveMatrix() const;

private:
	Transform _Transform;

	float _FOV = 60.f;
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
	// ���� ���. ���� ���� ������ -1~1
	float invA = 1.f / _ViewportSize.AspectRatio();

	float d = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);
	return Matrix4x4(
		Vector4::UnitX * invA * d,
		Vector4::UnitY * d,
		Vector4(0.f, 0.f, -1.f, 0.f),
		Vector4::UnitW);
}

}
}