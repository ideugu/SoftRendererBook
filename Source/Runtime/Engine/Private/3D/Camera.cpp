
#include "Precompiled.h"
using namespace CK::DDD;

void Camera::SetLookAtRotation(const GameObject& InGameObject, const Vector3& InUp)
{
	SetLookAtRotation(InGameObject.GetTransform().GetWorldPosition(), InUp);
}


void Camera::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	_Transform.SetWorldRotation(Quaternion(InTargetPosition - _Transform.GetWorldPosition()));
}
