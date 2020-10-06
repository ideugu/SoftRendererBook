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
	// ���� ��带 �θ�κ��� �и� ( ���� = ���� )
	if (!SetRoot())
	{
		return false;
	}

	// ���ο� �θ��� �ڽ����� ���. �̹� �ִ� ��쿡�� ������ �ִ� ��Ȳ.
	auto it = std::find(InTransformNode.ChildBegin(), InTransformNode.ChildEnd(), this);
	if (it != InTransformNode.ChildEnd())
	{
		return false;
	}

	// ���ο� Ʈ������ ���� �θ� �缳��
	InTransformNode.GetChildren().emplace_back(this);
	_ParentPtr = &InTransformNode;
	TransformNode& newParent = *_ParentPtr;

	// �ڽ��� ���ð� ��� �ڽ��� ���带 ������Ʈ�Ѵ�.
	UpdateLocal();

	return true;
}

// ���� ����, Ȥ�� �θ� ����Ǹ� �̸� ������� ���� ������ ����
void TransformNode::UpdateLocal()
{
	if (HasParent())
	{
		const TransformNode& parent = *GetParentPtr();
		_LocalTransform = _WorldTransform.WorldToLocal(parent.GetWorldTransform());
	}
	else
	{
		_LocalTransform = _WorldTransform;
	}

	// ���� ���� ���� �� �ڽ��� ���� ������ ������Ʈ ( ���� ������ ���� ����. )
	UpdateChildrenWorld();
}

// ���� ������ ������Ʈ �Ǿ ���� ������ �ٽ� ���
void TransformNode::UpdateWorld()
{
	// �ڽ��� ���� ���� ������Ʈ
	if (HasParent())
	{
		const TransformNode& parent = *GetParentPtr();
		_WorldTransform = _LocalTransform.LocalToWorld(parent.GetWorldTransform());
	}
	else
	{
		_WorldTransform = _LocalTransform;
	}

	// ���� ���� ���� �� �ڽ��� ���� ������ ������Ʈ ( ���� ������ ���� ����. )
	UpdateChildrenWorld();
}

void TransformNode::UpdateChildrenWorld()
{
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->UpdateWorld();
	}
}

