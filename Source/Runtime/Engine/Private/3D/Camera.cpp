
#include "Precompiled.h"
using namespace CK::DDD;

void Camera::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	Vector3 localX, localY, localZ;

	// ���� Z�� ����
	localZ = (InTargetPosition - _Transform.GetPosition()).Normalize();
	// �ü� ����� ���� Y���� ������ ���
	if (Math::Abs(localZ.Y) >= (1.f - KINDA_SMALL_NUMBER))
	{
		// Ư�� ��Ȳ���� ���� X ��ǥ ���� ���Ƿ� ����.
		localX = Vector3::UnitX;
	}
	else
	{
		localX = InUp.Cross(localZ).Normalize();
	}
	localY = localZ.Cross(localX).Normalize();

	_Transform.SetLocalAxes(localX, localY, localZ);
}
