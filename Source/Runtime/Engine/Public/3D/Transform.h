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
		//Vector3 Scale3D;

		//const float SquareSum0 = InMatrix[0].ToVector3().SizeSquared();
		//const float SquareSum1 = InMatrix[1].ToVector3().SizeSquared();
		//const float SquareSum2 = InMatrix[2].ToVector3().SizeSquared();

		//if (SquareSum0 > SMALL_NUMBER)
		//{
		//	float Scale0 = sqrtf(SquareSum0);
		//	Scale3D[0] = Scale0;
		//}
		//else
		//{
		//	Scale3D[0] = 0;
		//}

		//if (SquareSum1 > Tolerance)
		//{
		//	float Scale1 = FMath::Sqrt(SquareSum1);
		//	Scale3D[1] = Scale1;
		//}
		//else
		//{
		//	Scale3D[1] = 0;
		//}

		//if (SquareSum2 > Tolerance)
		//{
		//	float Scale2 = FMath::Sqrt(SquareSum2);
		//	Scale3D[2] = Scale2;
		//	float InvScale2 = 1.f / Scale2;
		//	M[2][0] *= InvScale2;
		//	M[2][1] *= InvScale2;
		//	M[2][2] *= InvScale2;
		//}
		//else
		//{
		//	Scale3D[2] = 0;
		//}

		//return Scale3D;
	}

public: // 트랜스폼 설정함수
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
