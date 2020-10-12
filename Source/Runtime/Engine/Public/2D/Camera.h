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
	// Ʈ������
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransform() const { return _Transform; }

	// ī�޶� ���� �������� �Լ�
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// ī�޶� ���� �����ϴ� �Լ�
	void SetViewportSize(const ScreenPoint & InViewportSize) { _ViewportSize = InViewportSize; }

	// ��� ����
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