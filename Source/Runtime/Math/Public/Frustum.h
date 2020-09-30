#pragma once

namespace CK
{

struct Frustum
{
	// 생성자
	Frustum() = default;
	Frustum(const std::array<Plane, 6>& InPlanes);

	// 멤버함수 
	FORCEINLINE bool IsOutside(const Vector3& InPoint) const;
	FORCEINLINE bool IsOutside(const Sphere& InSphere) const;
	FORCEINLINE bool IsIntersect(const Sphere& InSphere) const;

	// 멤버변수 	
	std::array<Plane, 6> Planes; // Y+, Y-, X+, X-, Z+, Z- 순으로 저장
};

FORCEINLINE bool Frustum::IsOutside(const Vector3& InPoint) const
{
	for (const auto& p : Planes)
	{
		if (p.IsOutside(InPoint))
			return true;
	}

	return false;
}

FORCEINLINE bool Frustum::IsOutside(const Sphere& InSphere) const
{
	for (const auto& p : Planes)
	{
		if (p.Distance(InSphere.Center) > InSphere.Radius)
		{
			return true;
		}
	}

	return false;
}

FORCEINLINE bool Frustum::IsIntersect(const Sphere& InSphere) const
{
	for (const auto& p : Planes)
	{
		if (Math::Abs(p.Distance(InSphere.Center)) <= InSphere.Radius)
		{
			return true;
		}
	}

	return false;
}


}