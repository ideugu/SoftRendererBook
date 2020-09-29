#pragma once

namespace CK
{

struct Plane
{
public:
	Plane() = default;
	Plane(const Vector3& InNormal, float InDistance) : Normal(InNormal), D(InDistance)  { Normalize(); }
	Plane(const Vector3& InNormal, Vector3 InPlanePoint) : Normal(InNormal) 
	{ 
		Normal.Normalize();
		D = -Normal.Dot(InPlanePoint);
	}
	Plane(const Vector3& InPoint1, const Vector3& InPoint2, const Vector3& InPoint3)
	{
		Vector3 v1 = InPoint2 - InPoint1;
		Vector3 v2 = InPoint3 - InPoint1;

		Normal = v1.Cross(v2).Normalize();
		D = -Normal.Dot(InPoint1);
	}

	float Distance(const Vector3& InPoint) const;
	void Normalize();

public:
	Vector3 Normal = Vector3::UnitY;
	float D = 0.f;
};

}
