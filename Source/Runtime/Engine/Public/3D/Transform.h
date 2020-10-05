#pragma once

namespace CK
{
namespace DDD
{

struct Transform
{
public:
	Transform() = default;
	Transform(const Vector3& InPosition) : Position(InPosition) { }
	Transform(const Vector3& InPosition, const Quaternion& InRotation) : Position(InPosition), Rotation(InRotation) { }
	Transform(const Vector3& InPosition, const Quaternion& InRotation, const Vector3& InScale) : Position(InPosition), Rotation(InRotation), Scale(InScale) { }

public: // 트랜스폼 설정함수
	void SetPosition(const Vector3& InPosition) { Position = InPosition; }
	void AddPosition(const Vector3& InDeltaPosition) { Position += InDeltaPosition; }
	void AddYawRotation(float InDegree) { Rotation *= Quaternion(Vector3::UnitY, InDegree); }
	void AddRollRotation(float InDegree) { Rotation *= Quaternion(Vector3::UnitZ, InDegree); }
	void AddPitchRotation(float InDegree) { Rotation *= Quaternion(Vector3::UnitX, InDegree); }
	void SetRotation(const Rotator& InRotator) { Rotation = Quaternion(InRotator); }
	void SetRotation(const Matrix3x3& InMatrix) { Rotation = Quaternion(InMatrix); }
	void SetRotation(const Quaternion& InQuaternion) { Rotation = InQuaternion; }
	void SetScale(const Vector3& InScale) { Scale = InScale; }

	FORCEINLINE Vector3 GetXAxis() const { return Rotation * Vector3::UnitX; }
	FORCEINLINE Vector3 GetYAxis() const { return Rotation * Vector3::UnitY; }
	FORCEINLINE Vector3 GetZAxis() const { return Rotation * Vector3::UnitZ; }
	Matrix4x4 GetMatrix() const;

	FORCEINLINE Vector3 GetPosition() const { return Position; }
	FORCEINLINE Quaternion GetRotation() const { return Rotation; }
	FORCEINLINE Vector3 GetScale() const { return Scale; }

private: // 트랜스폼에 관련된 변수
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scale = Vector3::One;

};

FORCEINLINE Matrix4x4 Transform::GetMatrix() const
{
	return Matrix4x4(
		Vector4(GetXAxis() * Scale.X, false),
		Vector4(GetYAxis() * Scale.Y, false),
		Vector4(GetZAxis() * Scale.Z, false),
		Vector4(Position, true)
	);
}

}
}
