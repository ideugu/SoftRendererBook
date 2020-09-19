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
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;
	FORCEINLINE Matrix4x4 GetPerspectiveMatrix() const;
	void SetLookAtRotation(const Vector3& InTargetPosition);
	void SetAspectRatio(float InAspectRatio) { _AspectRatio = InAspectRatio; }

private:
	Transform _Transform;

	float _FOV = 60.f;
	float _NearZ = 5.5f;
	float _FarZ = 1000.f;
	float _AspectRatio = 1.333f;  // Y축 기준
};

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	const Vector3& localX = _Transform.GetLocalX();
	const Vector3& localY = _Transform.GetLocalY();
	const Vector3& localZ = _Transform.GetLocalZ();
	Vector3 pos = _Transform.GetPosition();
	return Matrix4x4(
		Vector4(localX.X, localY.X, localZ.X, 0.f),
		Vector4(localX.Y, localY.Y, localZ.Y, 0.f),
		Vector4(localX.Z, localY.Z, localZ.Z, 0.f),
		Vector4(-localX.Dot(pos), -localY.Dot(pos), -localZ.Dot(pos), 1.f)
	);
}

FORCEINLINE Matrix4x4 Camera::GetViewMatrixRotationOnly() const
{
	const Vector3& localX = _Transform.GetLocalX();
	const Vector3& localY = _Transform.GetLocalY();
	const Vector3& localZ = _Transform.GetLocalZ();
	return Matrix4x4(
		Vector4(-localX.X, localY.X, -localZ.X, 0.f),
		Vector4(-localX.Y, localY.Y, -localZ.Y, 0.f),
		Vector4(-localX.Z, localY.Z, -localZ.Z, 0.f),
		Vector4::UnitW
	);
}

FORCEINLINE Matrix4x4 Camera::GetPerspectiveMatrix() const
{
	// 투영 행렬. 깊이 값의 범위는 -1~1
	float invA = 1.f / _AspectRatio;

	float d = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);
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