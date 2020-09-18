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
	FORCEINLINE Matrix4x4 GetPerspectiveMatrix(int InScreenSizeX, int InScreenSizeY) const;
	void SetLookAtRotation(const Vector3& InTargetPosition);

private:
	Transform _Transform;

	float _FOV = 60.f;
	float _NearZ = 5.5f;
	float _FarZ = 1000.f;
};

FORCEINLINE Matrix4x4 Camera::GetViewMatrix() const
{
	Matrix4x4 inverseRotation = Matrix4x4(Vector4(_Transform.GetLocalX(), false), Vector4(_Transform.GetLocalY(), false), Vector4(_Transform.GetLocalZ(), false), Vector4::UnitW).Tranpose();
	Matrix4x4 inverseTranslate = Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-_Transform.GetPosition()));
	return inverseRotation * inverseTranslate;
}

FORCEINLINE Matrix4x4 Camera::GetPerspectiveMatrix(int InScreenSizeX, int InScreenSizeY) const
{
	// 투영 행렬. 깊이 값의 범위는 -1~1
	float invA = (float)InScreenSizeY / (float)InScreenSizeX;

	float focalLength = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);
	float invNF = 1.f / (_NearZ - _FarZ);
	float k = (_FarZ + _NearZ) * invNF;
	float l = 2.f * _FarZ * _NearZ * invNF;
	return Matrix4x4(
		Vector4::UnitX * invA * focalLength,
		Vector4::UnitY * focalLength,
		Vector4(0.f, 0.f, k, -1.f),
		Vector4(0.f, 0.f, l, 0.f));

}

}
}