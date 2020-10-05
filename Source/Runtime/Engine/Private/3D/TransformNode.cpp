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
	UpdateLocal();
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
	// 현재 노드를 부모로부터 분리 ( 월드 = 로컬 )
	if (!SetRoot())
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
	UpdateLocal();

	return true;
}

// 월드 정보, 혹은 부모가 변경되면 이를 기반으로 로컬 정보를 변경
void TransformNode::UpdateLocal()
{
	if (HasParent())
	{
		const TransformNode& parent = *GetParentPtr();
		const Vector3& parentWorldScale = parent.GetWorldScale();
		const Quaternion& parentWorldRotation = parent.GetWorldRotation();
		const Vector3& parentWorldPosition = parent.GetWorldPosition();

		Vector3 invScale = Vector3(1.f / parentWorldScale.X, 1.f / parentWorldScale.Y, 1.f / parentWorldScale.Z);
		Quaternion invRotation = parentWorldRotation.Inverse();

		_LocalTransform.SetScale(GetWorldScale() * invScale);
		_LocalTransform.SetRotation(invRotation * GetWorldRotation());

		Vector3 translatedVector = GetWorldPosition() - parentWorldPosition;
		Vector3 rotatedVector = invRotation.RotateVector(translatedVector);
		_LocalTransform.SetPosition(rotatedVector * invScale);
	}
	else
	{
		_LocalTransform = _WorldTransform;
	}

	// 월드 정보 변경 시 자식의 월드 정보도 업데이트 ( 로컬 정보는 변함 없음. )
	UpdateChildrenWorld();
}

// 로컬 정보가 업데이트 되어서 월드 정보만 다시 계산
void TransformNode::UpdateWorld()
{
	// 자신의 월드 정보 업데이트
	if (HasParent())
	{
		const TransformNode& parent = *GetParentPtr();
		_WorldTransform = _LocalTransform * parent.GetWorldTransform();
	}
	else
	{
		_WorldTransform = _LocalTransform;
	}

	// 월드 정보 변경 시 자식의 월드 정보도 업데이트 ( 로컬 정보는 변함 없음. )
	UpdateChildrenWorld();
}

void TransformNode::UpdateChildrenWorld()
{
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->UpdateWorld();
	}
}

