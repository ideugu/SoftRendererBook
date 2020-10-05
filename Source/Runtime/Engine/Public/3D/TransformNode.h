#pragma once

namespace CK
{
namespace DDD
{

class TransformNode
{
public:
	TransformNode() = default;
	TransformNode(const Transform& InLocalTransform) : _LocalTransform(InLocalTransform) 
	{ 
		Update(); 
	}

public: // ���� Ʈ������ ���� �Լ�
	FORCEINLINE void SetLocalPosition(const Vector3& InPosition);
	FORCEINLINE void AddLocalPosition(const Vector3& InDeltaPosition);
	FORCEINLINE void AddLocalYawRotation(float InDegree);
	FORCEINLINE void AddLocalRollRotation(float InDegree);
	FORCEINLINE void AddLocalPitchRotation(float InDegree);
	FORCEINLINE void SetLocalRotation(const Rotator& InRotator);
	FORCEINLINE void SetLocalRotation(const Quaternion& InQuaternion);
	FORCEINLINE void SetLocalRotation(const Matrix3x3& InMatrix);
	FORCEINLINE void SetLocalScale(const Vector3& InScale);

	FORCEINLINE Vector3 GetLocalPosition() const { return _LocalTransform.GetPosition(); }
	FORCEINLINE Rotator GetLocalRotator() const { return _LocalTransform.GetRotation().ToRotator(); }
	FORCEINLINE Quaternion GetLocalRotation() const { return _LocalTransform.GetRotation(); }
	FORCEINLINE Vector3 GetLocalScale() const { return _LocalTransform.GetScale(); }

	FORCEINLINE Vector3 GetLocalX() const { return _LocalTransform.GetXAxis(); }
	FORCEINLINE Vector3 GetLocalY() const { return _LocalTransform.GetYAxis(); }
	FORCEINLINE Vector3 GetLocalZ() const { return _LocalTransform.GetZAxis(); }
	FORCEINLINE Matrix4x4 GetLocalMatrix() const { return _LocalTransform.GetMatrix(); }

public: // ���� Ʈ������ ���� �Լ�
	FORCEINLINE void SetWorldPosition(const Vector3& InPosition);
	FORCEINLINE void AddWorldPosition(const Vector3& InDeltaPosition);
	FORCEINLINE void AddWorldYawRotation(float InDegree);
	FORCEINLINE void AddWorldRollRotation(float InDegree);
	FORCEINLINE void AddWorldPitchRotation(float InDegree);
	FORCEINLINE void SetWorldRotation(const Rotator& InRotator);
	FORCEINLINE void SetWorldRotation(const Matrix3x3& InMatrix);
	FORCEINLINE void SetWorldRotation(const Quaternion& InQuaternion);
	FORCEINLINE void SetWorldScale(const Vector3& InScale);

	FORCEINLINE Vector3 GetWorldPosition() const { return _WorldTransform.GetPosition(); }
	FORCEINLINE Rotator GetWorldRotator() const { return _WorldTransform.GetRotation().ToRotator(); }
	FORCEINLINE Quaternion GetWorldRotation() const { return _WorldTransform.GetRotation(); }
	FORCEINLINE Vector3 GetWorldScale() const { return _WorldTransform.GetScale(); }

	FORCEINLINE Matrix4x4 GetWorldMatrix() const { return _WorldTransform.GetMatrix(); }

public: // ���� ���� ���� �Լ�
	bool SetRoot();
	bool RemoveFromParent();
	TransformNode& GetRoot();
	bool SetParent(TransformNode& InTransformNode);

	FORCEINLINE bool HasParent() const { return _ParentPtr != nullptr; }
	std::vector<TransformNode*>& GetChildren() { return _ChildrenPtr; }
	std::vector<TransformNode*>::const_iterator ChildBegin() const { return _ChildrenPtr.begin(); }
	std::vector<TransformNode*>::const_iterator ChildEnd() const { return _ChildrenPtr.end(); }

private: // ���ο����� ȣ���ϴ� �Լ�
	FORCEINLINE TransformNode* GetParentPtr() const { return _ParentPtr; }
	void Update();

private: // ���� ������ ���� ����
	Transform _LocalTransform;
	Transform _WorldTransform;

	TransformNode* _ParentPtr = nullptr;
	std::vector<TransformNode*> _ChildrenPtr;
};


FORCEINLINE void TransformNode::SetLocalPosition(const Vector3& InPosition)
{
	_LocalTransform.SetPosition(InPosition);
	Update();
}

FORCEINLINE void TransformNode::AddLocalPosition(const Vector3& InDeltaPosition)
{
	_LocalTransform.AddPosition(InDeltaPosition);
	Update();
}

FORCEINLINE void TransformNode::AddLocalYawRotation(float InDegree)
{
	_LocalTransform.AddYawRotation(InDegree);
	Update();
}

FORCEINLINE void TransformNode::AddLocalRollRotation(float InDegree)
{
	_LocalTransform.AddRollRotation(InDegree);
	Update();
}

FORCEINLINE void TransformNode::AddLocalPitchRotation(float InDegree)
{
	_LocalTransform.AddPitchRotation(InDegree);
	Update();
}

FORCEINLINE void TransformNode::SetLocalRotation(const Rotator& InRotator)
{
	_LocalTransform.SetRotation(InRotator);
	Update();
}

FORCEINLINE void TransformNode::SetLocalRotation(const Matrix3x3& InMatrix)
{
	_LocalTransform.SetRotation(InMatrix);
	Update();
}

FORCEINLINE void TransformNode::SetLocalRotation(const Quaternion& InQuaternion)
{
	_LocalTransform.SetRotation(InQuaternion);
	Update();
}

FORCEINLINE void TransformNode::SetLocalScale(const Vector3& InScale)
{
	_LocalTransform.SetScale(InScale);
	Update();
}

FORCEINLINE void TransformNode::SetWorldPosition(const Vector3& InPosition)
{
	_WorldTransform.SetPosition(InPosition);
	Update();
}

FORCEINLINE void TransformNode::AddWorldPosition(const Vector3& InDeltaPosition)
{
	_WorldTransform.AddPosition(InDeltaPosition);
	Update();
}

FORCEINLINE void TransformNode::AddWorldYawRotation(float InDegree)
{
	_WorldTransform.AddYawRotation(InDegree);
	Update();
}

FORCEINLINE void TransformNode::AddWorldRollRotation(float InDegree)
{
	_WorldTransform.AddRollRotation(InDegree);
	Update();
}

FORCEINLINE void TransformNode::AddWorldPitchRotation(float InDegree)
{
	_WorldTransform.AddPitchRotation(InDegree);
	Update();
}

FORCEINLINE void TransformNode::SetWorldRotation(const Rotator& InRotator)
{
	_WorldTransform.SetRotation(InRotator);
	Update();
}

FORCEINLINE void TransformNode::SetWorldRotation(const Matrix3x3& InMatrix)
{
	_WorldTransform.SetRotation(InMatrix);
	Update();
}

FORCEINLINE void TransformNode::SetWorldRotation(const Quaternion& InQuaternion)
{
	_WorldTransform.SetRotation(InQuaternion);
	Update();
}

FORCEINLINE void TransformNode::SetWorldScale(const Vector3& InScale)
{
	_WorldTransform.SetScale(InScale);
	Update();
}


}
}
