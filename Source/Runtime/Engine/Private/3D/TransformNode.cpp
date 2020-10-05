#include "Precompiled.h"
using namespace CK::DDD;

bool TransformNode::RemoveFromParent()
{
	if (!HasParent())
	{
		return true;
	}

	TransformNode& parent = *GetParentPtr();
	auto it = std::find(parent.ChildBegin(), parent.ChildEnd(), this);
	if (it != parent.ChildEnd())
	{
		// 오류 발생.
		return false;
	}

	// 부모 트랜스폼에서 자식 정보를 제거
	parent.GetChildren().erase(it);

	// 자신에게서 부모 정보를 제거
	_ParentPtr = nullptr;
	return true;
}

bool TransformNode::SetRoot()
{
	if (!RemoveFromParent())
	{
		return false;
	}

	// 로컬 정보를 월드 정보로 변경
	_LocalTransform = _WorldTransform;
	return true;
}

FORCEINLINE TransformNode& TransformNode::GetRoot()
{
	TransformNode* parent = nullptr;
	TransformNode* current = this;
	while ((parent = current->GetParentPtr()) != nullptr)
	{
		current = parent;
	}

	return *current;
}

bool TransformNode::SetParent(TransformNode& InTransformNode)
{
	// 현재 노드를 부모로부터 분리
	if (!RemoveFromParent())
	{
		return false;
	}

	// 새로운 부모의 자식으로 등록. 이미 있는 경우에는 문제가 있는 상황.
	auto it = std::find(InTransformNode.ChildBegin(), InTransformNode.ChildEnd(), this);
	if (it != InTransformNode.ChildEnd())
	{
		return false;
	}

	InTransformNode.GetChildren().emplace_back(this);
	_ParentPtr = &InTransformNode;
	TransformNode& newParent = *_ParentPtr;

	const Vector3& parentWorldScale = newParent.GetWorldScale();
	const Quaternion& parentWorldRotation = newParent.GetWorldRotation();
	const Vector3& parentWorldPosition = newParent.GetWorldPosition();

	Vector3 invScale = Vector3(1.f / parentWorldScale.X, 1.f / parentWorldScale.Y, 1.f / parentWorldScale.Z);
	Quaternion invRotation = parentWorldRotation.Inverse();

	_LocalTransform.SetScale(GetLocalScale() * invScale);
	_LocalTransform.SetRotation(invRotation * GetLocalRotation());

	Vector3 translatedVector = GetLocalPosition() - parentWorldPosition;
	Vector3 rotatedVector = invRotation.RotateVector(translatedVector);
	_LocalTransform.SetPosition(rotatedVector * invScale);
	Update();

	return true;
}

void TransformNode::Update()
{
	// 자신의 월드 정보 업데이트
	if (HasParent())
	{
		const TransformNode& parent = *GetParentPtr();
		_WorldTransform.SetScale(parent.GetWorldScale() * GetLocalScale());
		_WorldTransform.SetRotation(parent.GetWorldRotation() * GetLocalRotation());
		_WorldTransform.SetPosition(parent.GetWorldMatrix() * GetLocalPosition());
	}
	else
	{
		_WorldTransform = _LocalTransform;
	}

	// 자식의 트랜스폼 정보 업데이트
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->Update();
	}
}