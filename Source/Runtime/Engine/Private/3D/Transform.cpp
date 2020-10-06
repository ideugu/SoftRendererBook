#include "Precompiled.h"
using namespace CK::DDD;

bool Transform::RemoveFromParent()
{
	if (!HasParent())
	{
		return true;
	}

	Transform& parent = *GetParentPtr();
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

bool Transform::SetRoot()
{
	if (!RemoveFromParent())
	{
		return false;
	}

	// ���� ������ ���� ������ ����
	UpdateLocal();
	return true;
}

FORCEINLINE Transform& Transform::GetRoot()
{
	Transform* parent = nullptr;
	Transform* current = this;
	while ((parent = current->GetParentPtr()) != nullptr)
	{
		current = parent;
	}

	return *current;
}

bool Transform::SetParent(Transform& InTransform)
{
	// ���� ��带 �θ�κ��� �и� ( ���� = ���� )
	if (!SetRoot())
	{
		return false;
	}

	// ���ο� �θ��� �ڽ����� ���. �̹� �ִ� ��쿡�� ������ �ִ� ��Ȳ.
	auto it = std::find(InTransform.ChildBegin(), InTransform.ChildEnd(), this);
	if (it != InTransform.ChildEnd())
	{
		return false;
	}

	// ���ο� Ʈ������ ���� �θ� �缳��
	InTransform.GetChildren().emplace_back(this);
	_ParentPtr = &InTransform;
	Transform& newParent = *_ParentPtr;

	// �ڽ��� ���ð� ��� �ڽ��� ���带 ������Ʈ�Ѵ�.
	UpdateLocal();

	return true;
}

// ���� ����, Ȥ�� �θ� ����Ǹ� �̸� ������� ���� ������ ����
void Transform::UpdateLocal()
{
	if (HasParent())
	{
		const Transform& parent = *GetParentPtr();
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
void Transform::UpdateWorld()
{
	// �ڽ��� ���� ���� ������Ʈ
	if (HasParent())
	{
		const Transform& parent = *GetParentPtr();
		_WorldTransform = _LocalTransform.LocalToWorld(parent.GetWorldTransform());
	}
	else
	{
		_WorldTransform = _LocalTransform;
	}

	// ���� ���� ���� �� �ڽ��� ���� ������ ������Ʈ ( ���� ������ ���� ����. )
	UpdateChildrenWorld();
}

void Transform::UpdateChildrenWorld()
{
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->UpdateWorld();
	}
}

