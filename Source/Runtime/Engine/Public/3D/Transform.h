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
	Transform(const Matrix4x4& InMatrix)
	{ 
		// ������ ȸ�� ��ĸ� �и�
		Matrix3x3 rotScaleMatrix = InMatrix.ToMatrix3x3();
		Position = InMatrix[3].ToVector3();

		// ũ�� ��ĺ��� ���Ѵ�. 
		Scale = Vector3::Zero;
		const float squareSumX = rotScaleMatrix[0].SizeSquared();
		const float squareSumY = rotScaleMatrix[1].SizeSquared();
		const float squareSumZ = rotScaleMatrix[2].SizeSquared();
		if (squareSumX > SMALL_NUMBER) { Scale.X = sqrtf(squareSumX); }
		if (squareSumY > SMALL_NUMBER) { Scale.Y = sqrtf(squareSumY); }
		if (squareSumZ > SMALL_NUMBER) { Scale.Z = sqrtf(squareSumZ); }

		// ũ�� ��Ҹ� ���� ���� ���� ����� ���Ѵ�.
		rotScaleMatrix[0] /= squareSumX;
		rotScaleMatrix[1] /= squareSumY;
		rotScaleMatrix[2] /= squareSumZ;

		// ������� ��ȯ�Ѵ�.
		Rotation = Quaternion(rotScaleMatrix);
	}

public: // Ʈ������ �����Լ�
	void SetPosition(const Vector3& InPosition) { Position = InPosition; }
	void AddPosition(const Vector3& InDeltaPosition) { Position += InDeltaPosition; }
	void AddYawRotation(float InDegree) 
	{
		Rotator r = Rotation.ToRotator();
		r.Yaw += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}
	void AddRollRotation(float InDegree) 
	{ 
		Rotator r = Rotation.ToRotator();
		r.Roll += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}
	void AddPitchRotation(float InDegree) 
	{ 
		Rotator r = Rotation.ToRotator();
		r.Pitch += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}

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

	// ������
	FORCEINLINE Transform operator*(const Transform& InTransform) const;

private: // Ʈ�������� ���õ� ����
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

FORCEINLINE Transform Transform::operator*(const Transform& InTransform) const
{
	Transform result;
	result.Rotation = InTransform.Rotation * Rotation;
	result.Scale = Scale * InTransform.Scale;
	result.Position = InTransform.Rotation.RotateVector(InTransform.Scale * Position) + InTransform.Position;
	return result;
}

}
}
