#pragma once

namespace CK
{
namespace DDD
{

class Transform
{
public:
	Transform() = default;

public:
	void SetPosition(const Vector3& InPosition) { _Position = InPosition; }
	void AddPosition(const Vector3& InDeltaPosition) { _Position += InDeltaPosition; }
	void AddYawRotation(float InDegree) { _Rotation.Yaw += InDegree; _Rotation.Clamp(); CalculateLocalAxis(); }
	void AddRollRotation(float InDegree) { _Rotation.Roll += InDegree; _Rotation.Clamp(); CalculateLocalAxis(); }
	void AddPitchRotation(float InDegree) { _Rotation.Pitch += InDegree; _Rotation.Clamp(); CalculateLocalAxis(); }
	void SetRotation(const Rotator& InRotation) { _Rotation = InRotation; CalculateLocalAxis(); }
	void SetScale(const Vector3& InScale) { _Scale = InScale; }

	Vector3 GetPosition() const { return _Position; }
	Rotator GetRotation() const { return _Rotation; }
	Vector3 GetScale() const { return _Scale; }

	Matrix4x4 GetModelingMatrix() const;
	const Vector3& GetLocalX() const { return _Right; }
	const Vector3& GetLocalY() const { return _Up; }
	const Vector3& GetLocalZ() const { return _Forward; }

private:
	void CalculateLocalAxis();

	Vector3 _Position = Vector3::Zero;
	Rotator _Rotation;
	Vector3 _Scale = Vector3::One;

	Vector3 _Right = Vector3::UnitX;
	Vector3 _Up = Vector3::UnitY;
	Vector3 _Forward = Vector3::UnitZ;

	friend class GameObject;
	friend class Camera;
};

}
}
