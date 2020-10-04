#pragma once

namespace CK
{
namespace DDD
{

class Transform
{
public:
	Transform() = default;
	Transform(const Vector3& InPosition) : _Position(InPosition) { Update(); }
	Transform(const Vector3& InPosition, const Quaternion& InRotation) : _Position(InPosition), _Rotation(InRotation) { Update(); }
	Transform(const Vector3& InPosition, const Quaternion& InRotation, const Vector3& InScale) : _Position(InPosition), _Rotation(InRotation), _Scale(InScale) { Update(); }

public:
	FORCEINLINE void SetPosition(const Vector3& InPosition) { _Position = InPosition; Update(); }
	void AddPosition(const Vector3& InDeltaPosition) { _Position += InDeltaPosition; Update(); }
	void AddYawRotation(float InDegree) { _Rotation *= Quaternion(Vector3::UnitY, InDegree); Update(); }
	void AddRollRotation(float InDegree) { _Rotation *= Quaternion(Vector3::UnitZ, InDegree); Update(); }
	void AddPitchRotation(float InDegree) { _Rotation *= Quaternion(Vector3::UnitX, InDegree); Update(); }
	void SetRotation(const Rotator& InRotator) { _Rotation = Quaternion(InRotator); Update(); }
	void SetRotation(const Quaternion& InQuaternion) { _Rotation = InQuaternion; Update(); }
	void SetScale(const Vector3& InScale) { _Scale = InScale; Update(); }

	FORCEINLINE Vector3 GetPosition() const { return _Position; }
	Rotator GetRotator() const { return _Rotation.ToRotator(); }
	FORCEINLINE Quaternion GetRotation() const { return _Rotation; }
	FORCEINLINE Vector3 GetScale() const { return _Scale; }

	FORCEINLINE Matrix4x4 GetLocalMatrix() const;
	const Vector3& GetLocalX() const { return _Right; }
	const Vector3& GetLocalY() const { return _Up; }
	const Vector3& GetLocalZ() const { return _Forward; }
	void SetLocalAxes(const Vector3& InRight, const Vector3& InUp, const Vector3& InForward)
	{
		_Right = InRight;
		_Up = InUp;
		_Forward = InForward;
	}

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
	FORCEINLINE Transform& GetChild(BYTE InIndex) const
	{
		assert(InIndex < _Children.size());
		return *_Children[InIndex];
	}

	bool SetParent(Transform& InTransform);

	// 월드 트랜스폼 정보
	FORCEINLINE const Vector3& GetWorldPosition() const { return _WorldPosition; }
	FORCEINLINE const Quaternion& GetWorldRotation() const { return _WorldRotation; }
	FORCEINLINE const Vector3& GetWorldScale() const { return _WorldScale; }
	FORCEINLINE const Matrix4x4& GetWorldMatrix() const { return _WorldMatrix; }

private:
	FORCEINLINE Transform* GetParentPtr() const { return _ParentPtr; }
	FORCEINLINE void Update();

private:
	Vector3 _Position = Vector3::Zero;
	Quaternion _Rotation;
	Vector3 _Scale = Vector3::One;

	Vector3 _Right = Vector3::UnitX;
	Vector3 _Up = Vector3::UnitY;
	Vector3 _Forward = Vector3::UnitZ;

private: // 계층 구조를 위한 변수
	Vector3 _WorldPosition = Vector3::Zero;
	Quaternion _WorldRotation = Quaternion::Identity;
	Vector3 _WorldScale = Vector3::One;

	Matrix4x4 _WorldMatrix;

	Transform* _ParentPtr = nullptr;
	std::vector<Transform*> _Children;
};

FORCEINLINE Matrix4x4 Transform::GetLocalMatrix() const
{
	return Matrix4x4(
		Vector4(_Right * _Scale.X, false),
		Vector4(_Up * _Scale.Y, false),
		Vector4(_Forward * _Scale.Z, false),
		Vector4(_Position, true)
	);
}

FORCEINLINE void Transform::Update()
{
	// 로컬 축 업데이트
	_Right = _Rotation * Vector3::UnitX;
	_Up = _Rotation * Vector3::UnitY;
	_Forward = _Rotation * Vector3::UnitZ;

	// 월드 정보 업데이트
	if (HasParent())
	{
		Transform& parent = *GetParentPtr();
		_WorldScale = parent.GetWorldScale() * _Scale;
		_WorldRotation = parent.GetWorldRotation() * _Rotation;
		_WorldPosition = parent.GetWorldMatrix() * _Position;
		_WorldMatrix = parent.GetWorldMatrix() * GetLocalMatrix();
	}
	else
	{
		_WorldScale = _Scale;
		_WorldRotation = _Rotation;
		_WorldPosition = _Position;
		_WorldMatrix = GetLocalMatrix();
	}

	// 자식의 월드 정보 업데이트
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->Update();
	}
}

}
}
