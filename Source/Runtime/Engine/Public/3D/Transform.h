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
	void AddYawRotation(float InDegree) { _Rotation *= Quaternion(Vector3::UnitY, InDegree); CalculateLocalAxis(); }
	void AddRollRotation(float InDegree) { _Rotation *= Quaternion(Vector3::UnitZ, InDegree); CalculateLocalAxis(); }
	void AddPitchRotation(float InDegree) { _Rotation *= Quaternion(Vector3::UnitX, InDegree); CalculateLocalAxis(); }
	void SetRotation(const Rotator& InRotator) { _Rotation = Quaternion(InRotator); CalculateLocalAxis(); }
	void SetRotation(const Quaternion& InQuaternion) { _Rotation = InQuaternion; CalculateLocalAxis(); }
	void SetScale(const Vector3& InScale) { _Scale = InScale; }

	Vector3 GetPosition() const { return _Position; }
	Rotator GetRotator() const { return _Rotation.ToRotator(); }
	Quaternion GetRotation() const { return _Rotation; }
	Vector3 GetScale() const { return _Scale; }

	FORCEINLINE Matrix4x4 GetModelingMatrix() const;
	const Vector3& GetLocalX() const { return _Right; }
	const Vector3& GetLocalY() const { return _Up; }
	const Vector3& GetLocalZ() const { return _Forward; }
	void SetLocalAxes(const Vector3& InRight, const Vector3& InUp, const Vector3& InForward)
	{
		_Right = InRight;
		_Up = InUp;
		_Forward = InForward;
	}

private:
	FORCEINLINE void CalculateLocalAxis();

	Vector3 _Position = Vector3::Zero;
	Quaternion _Rotation;
	Vector3 _Scale = Vector3::One;

	Vector3 _Right = Vector3::UnitX;
	Vector3 _Up = Vector3::UnitY;
	Vector3 _Forward = Vector3::UnitZ;
};

FORCEINLINE Matrix4x4 Transform::GetModelingMatrix() const
{
	Matrix4x4 translate = Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(_Position, true));
	Matrix4x4 rotate = Matrix4x4(Vector4(_Right, false), Vector4(_Up, false), Vector4(_Forward, false), Vector4::UnitW);
	Matrix4x4 scale = Matrix4x4(Vector4::UnitX * _Scale.X, Vector4::UnitY * _Scale.Y, Vector4::UnitZ * _Scale.Z, Vector4::UnitW);
	return translate * rotate * scale;
}

FORCEINLINE void Transform::CalculateLocalAxis()
{
	_Right = _Rotation * Vector3::UnitX;
	_Up = _Rotation * Vector3::UnitY;
	_Forward = _Rotation * Vector3::UnitZ;
}


}
}
