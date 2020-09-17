#pragma once

namespace CK
{
namespace DD
{ 

class Transform
{
public:
	Transform() = default;

public:
	// 게임 로직에서 사용할 멤버 함수
	void SetPosition(const Vector2& InPosition) { Position = InPosition; }
	void AddPosition(const Vector2& InDeltaPosition) { Position += InDeltaPosition; }
	void SetScale(const Vector2& InScale) { Scale = InScale; }
	void SetRotation(float InDegree) { Rotation = InDegree; CalculateLocalAxis(); }
	void AddRotation(float InDegree) { Rotation += InDegree; CalculateLocalAxis(); }
	Vector2 GetPosition() const { return Position; }
	Vector2 GetScale() const { return Scale; }
	float GetRotation() const { return Rotation; }
	const Vector2& GetLocalX() const { return Right; }
	const Vector2& GetLocalY() const { return Up; }

	// 렌더링 로직에서 사용할 행렬 생성 함수
	Matrix3x3 GetModelingMatrix() const;

private:
	void CalculateLocalAxis();

private:
	Vector2 Position = Vector2::Zero;
	float Rotation = 0.f;
	Vector2 Scale = Vector2::One;

	Vector2 Right = Vector2::UnitX;
	Vector2 Up = Vector2::UnitY;
};

}
}