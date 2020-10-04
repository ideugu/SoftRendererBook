#pragma once

namespace CK
{
namespace DDD
{

class Transform
{
public:
	Transform() = default;
	Transform(const Vector3& InPosition) : _LocalPosition(InPosition) { Update(); }
	Transform(const Vector3& InPosition, const Quaternion& InRotation) : _LocalPosition(InPosition), _LocalRotation(InRotation) { Update(); }
	Transform(const Vector3& InPosition, const Quaternion& InRotation, const Vector3& InScale) : _LocalPosition(InPosition), _LocalRotation(InRotation), _LocalScale(InScale) { Update(); }

public: // 로컬 트랜스폼 정보
	void SetLocalPosition(const Vector3& InPosition) { _LocalPosition = InPosition; Update(); }
	void AddLocalPosition(const Vector3& InDeltaPosition) { _LocalPosition += InDeltaPosition; Update(); }
	void AddLocalYawRotation(float InDegree) { _LocalRotation *= Quaternion(Vector3::UnitY, InDegree); Update(); }
	void AddLocalRollRotation(float InDegree) { _LocalRotation *= Quaternion(Vector3::UnitZ, InDegree); Update(); }
	void AddLocalPitchRotation(float InDegree) { _LocalRotation *= Quaternion(Vector3::UnitX, InDegree); Update(); }
	void SetLocalRotation(const Rotator& InRotator) { _LocalRotation = Quaternion(InRotator); Update(); }
	void SetLocalRotation(const Quaternion& InQuaternion) { _LocalRotation = InQuaternion; Update(); }
	void SetLocalScale(const Vector3& InScale) { _LocalScale = InScale; Update(); }

	Vector3 GetLocalPosition() const { return _LocalPosition; }
	Rotator GetLocalRotator() const { return _LocalRotation.ToRotator(); }
	Quaternion GetLocalRotation() const { return _LocalRotation; }
	Vector3 GetLocalScale() const { return _LocalScale; }

	Matrix4x4 GetLocalMatrix() const;
	const Vector3& GetLocalX() const { return _Right; }
	const Vector3& GetLocalY() const { return _Up; }
	const Vector3& GetLocalZ() const { return _Forward; }
	void SetLocalAxes(const Vector3& InRight, const Vector3& InUp, const Vector3& InForward)
	{
		_Right = InRight;
		_Up = InUp;
		_Forward = InForward;
	}

public: // 월드 트랜스폼 정보
	const Vector3& GetWorldPosition() const { return _WorldPosition; }
	const Quaternion& GetWorldRotation() const { return _WorldRotation; }
	const Vector3& GetWorldScale() const { return _WorldScale; }

	FORCEINLINE Matrix4x4 GetWorldMatrix() const;

public: // 계층 구조 관련 함수
	FORCEINLINE bool HasParent() const { return _ParentPtr != nullptr; }
	std::vector<Transform*>& GetChildren() { return _Children; }
	std::vector<Transform*>::const_iterator ChildBegin() const { return _Children.begin(); }
	std::vector<Transform*>::const_iterator ChildEnd() const { return _Children.end(); }

	FORCEINLINE Transform& GetRoot() 
	{
		Transform* parent = nullptr;
		Transform* current = this;
		while ((parent = current->GetParentPtr()) != nullptr)
		{
			current = parent;
		}

		return *current;
	}
	bool SetParent(Transform& InTransform);

private:
	FORCEINLINE Transform* GetParentPtr() const { return _ParentPtr; }
	FORCEINLINE void Update();

private: // 로컬 공간에 관련된 변수
	Vector3 _LocalPosition = Vector3::Zero;
	Quaternion _LocalRotation;
	Vector3 _LocalScale = Vector3::One;

	Vector3 _Right = Vector3::UnitX;
	Vector3 _Up = Vector3::UnitY;
	Vector3 _Forward = Vector3::UnitZ;

private: // 월드 공간에 관련된 변수
	Vector3 _WorldPosition = Vector3::Zero;
	Quaternion _WorldRotation = Quaternion::Identity;
	Vector3 _WorldScale = Vector3::One;

private: // 계층 구조를 위한 변수
	Transform* _ParentPtr = nullptr;
	std::vector<Transform*> _Children;
};

FORCEINLINE Matrix4x4 Transform::GetLocalMatrix() const
{
	return Matrix4x4(
		Vector4(_Right * _LocalScale.X, false),
		Vector4(_Up * _LocalScale.Y, false),
		Vector4(_Forward * _LocalScale.Z, false),
		Vector4(_LocalPosition, true)
	);
}

FORCEINLINE Matrix4x4 Transform::GetWorldMatrix() const
{
	Vector3 worldRight = _WorldRotation * Vector3::UnitX;
	Vector3 worldUp = _WorldRotation * Vector3::UnitY;
	Vector3 worldForward = _WorldRotation * Vector3::UnitZ;

	return Matrix4x4(
		Vector4(worldRight * _WorldScale.X, false),
		Vector4(worldUp * _WorldScale.Y, false),
		Vector4(worldForward * _WorldScale.Z, false),
		Vector4(_WorldPosition, true)
	);
}

FORCEINLINE void Transform::Update()
{
	// 로컬 축 업데이트
	_Right = _LocalRotation * Vector3::UnitX;
	_Up = _LocalRotation * Vector3::UnitY;
	_Forward = _LocalRotation * Vector3::UnitZ;

	// 월드 정보 업데이트
	if (HasParent())
	{
		Transform& parent = *GetParentPtr();
		_WorldScale = parent.GetWorldScale() * _LocalScale;
		_WorldRotation = parent.GetWorldRotation() * _LocalRotation;
		_WorldPosition = parent.GetWorldMatrix() * _LocalPosition;
		//_WorldMatrix = parent.GetWorldMatrix() * GetLocalMatrix();
	}
	else
	{
		_WorldScale = _LocalScale;
		_WorldRotation = _LocalRotation;
		_WorldPosition = _LocalPosition;
		//_WorldMatrix = GetLocalMatrix();
	}

	// 자식의 월드 정보 업데이트
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->Update();
	}
}

}
}
