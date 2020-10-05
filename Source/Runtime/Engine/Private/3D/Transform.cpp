
#include "Precompiled.h"
using namespace CK::DDD;

//bool Transform::SetParent(Transform& InTransform)
//{
//	if (HasParent())
//	{
//		Transform& parent = *_ParentPtr;
//		auto it = std::find(parent.ChildBegin(), parent.ChildEnd(), this);
//		if (it != parent.ChildEnd())
//		{
//			return false;
//		}
//
//		// 부모 트랜스폼에서 자식 정보를 제거
//		parent.GetChildren().erase(it);
//
//		// 부모 정보를 제거
//		_ParentPtr = nullptr;
//	}
//
//	// 새로운 부모의 자식으로 등록.
//	auto it = std::find(InTransform.ChildBegin(), InTransform.ChildEnd(), this);
//	if (it != InTransform.ChildEnd())
//	{
//		return false;
//	}
//
//	InTransform.GetChildren().emplace_back(this);
//	_ParentPtr = &InTransform;
//	Transform& newParent = *_ParentPtr;
//
//	const Vector3& parentWorldScale = newParent.GetWorldScale();
//	const Quaternion& parentWorldRotation = newParent.GetWorldRotation();
//	const Vector3& parentWorldPosition = newParent.GetWorldPosition();
//
//	Vector3 invScale = Vector3(1.f / parentWorldScale.X, 1.f / parentWorldScale.Y, 1.f / parentWorldScale.Z);
//	Quaternion invRotation = parentWorldRotation.Inverse();
//
//	_LocalScale = _LocalScale * invScale;
//	_LocalRotation = invRotation * _LocalRotation;
//
//	Vector3 translatedVector = _LocalPosition - parentWorldPosition;
//	Vector3 rotatedVector = invRotation.RotateVector(translatedVector);
//	_LocalPosition = rotatedVector * invScale;
//
//	Update();
//	return true;
//}