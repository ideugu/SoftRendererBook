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
		// ���� �߻�.
		return false;
	}

	// �θ� Ʈ���������� �ڽ� ������ ����
	parent.GetChildren().erase(it);

	// �ڽſ��Լ� �θ� ������ ����
	_ParentPtr = nullptr;
	return true;
}

bool TransformNode::SetRoot()
{
	if (!RemoveFromParent())
	{
		return false;
	}

	// ���� ������ ���� ������ ����
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
	// ���� ��带 �θ�κ��� �и�
	if (!RemoveFromParent())
	{
		return false;
	}

	// ���ο� �θ��� �ڽ����� ���. �̹� �ִ� ��쿡�� ������ �ִ� ��Ȳ.
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
	// �ڽ��� ���� ���� ������Ʈ
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

	// �ڽ��� Ʈ������ ���� ������Ʈ
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->Update();
	}
}