
#include "Precompiled.h"
using namespace CK::DDD;

bool Transform::SetParent(Transform& InTransform)
{
	if (HasParent())
	{
		Transform& parent = *_ParentPtr;
		auto it = std::find(parent.ChildBegin(), parent.ChildEnd(), this);
		if (it != parent.ChildEnd())
		{
			return false;
		}

		// �θ� Ʈ���������� �ڽ� ������ ����
		parent.GetChildren().erase(it);

		// �θ� ������ ����
		_ParentPtr = nullptr;
	}

	// ���ο� �θ��� �ڽ����� ���.
	auto it = std::find(InTransform.ChildBegin(), InTransform.ChildEnd(), this);
	if (it != InTransform.ChildEnd())
	{
		return false;
	}

	InTransform.GetChildren().emplace_back(this);
	_ParentPtr = &InTransform;
	Transform& newParent = *_ParentPtr;

	const Vector3& parentWorldScale = newParent.GetWorldScale();
	const Quaternion& parentWorldRotation = newParent.GetWorldRotation();
	const Vector3& parentWorldPosition = newParent.GetWorldPosition();

	Vector3 invScale = Vector3(1.f / parentWorldScale.X, 1.f / parentWorldScale.Y, 1.f / parentWorldScale.Z);
	Quaternion invRotation = parentWorldRotation.Inverse();

	_Scale = _Scale * invScale;
	_Rotation = invRotation * _Rotation;

	Vector3 translatedVector = _Position - parentWorldPosition;
	Vector3 rotatedVector = invRotation.RotateVector(translatedVector);
	_Position = rotatedVector * invScale;

	Update();
	return true;
}