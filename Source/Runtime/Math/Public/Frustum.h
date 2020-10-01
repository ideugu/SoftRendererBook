#pragma once

namespace CK
{

struct Frustum
{
	// 생성자
	Frustum() = default;
	Frustum(const std::array<Plane, 6>& InPlanes);

	// 멤버함수 
	FORCEINLINE BoundCheckResult CheckBound(const Vector3& InPoint) const;
	FORCEINLINE BoundCheckResult CheckBound(const Sphere& InSphere) const;

	// 멤버변수 	
	std::array<Plane, 6> Planes; // Y+, Y-, X+, X-, Z+, Z- 순으로 저장
};

FORCEINLINE BoundCheckResult Frustum::CheckBound(const Vector3& InPoint) const
{
	for (const auto& p : Planes)
	{
		if (p.IsOutside(InPoint))
		{
			return BoundCheckResult::Outside;
		}
		else if (Math::EqualsInTolerance(p.Distance(InPoint), 0.f))
		{
			return BoundCheckResult::Intersect;
		}
	
	}

	return BoundCheckResult::Inside;
}

FORCEINLINE BoundCheckResult Frustum::CheckBound(const Sphere& InSphere) const
{
	for (const auto& p : Planes)
	{
		if (p.Distance(InSphere.Center) > InSphere.Radius)
		{
			return BoundCheckResult::Outside;
		}
		else if (Math::Abs(p.Distance(InSphere.Center)) <= InSphere.Radius)
		{
			return BoundCheckResult::Intersect;
		}

	}

	return BoundCheckResult::Inside;
}

}