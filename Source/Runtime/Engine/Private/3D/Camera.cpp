
#include "Precompiled.h"
using namespace CK::DDD;

void Camera::SetLookAtRotation(const GameObject& InGameObject, const Vector3& InUp)
{
	SetLookAtRotation(InGameObject.GetTransform().GetWorldPosition(), InUp);
}


void Camera::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	Vector3 viewX, viewY, viewZ;

	// ���� Z�� ����
	viewZ = (InTargetPosition - _Transform.GetWorldPosition()).Normalize();
	// �ü� ����� ���� Y���� ������ ���
	if (Math::Abs(viewZ.Y) >= (1.f - SMALL_NUMBER))
	{
		// Ư�� ��Ȳ���� ���� X ��ǥ ���� ���Ƿ� ����.
		viewX = Vector3::UnitX;
	}
	else
	{
		viewX = InUp.Cross(viewZ).Normalize();
	}
	viewY = viewZ.Cross(viewX).Normalize();

	_Transform.SetWorldRotation(Matrix3x3(viewX, viewY, viewZ));
}
