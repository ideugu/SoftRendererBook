#pragma once

namespace CK
{

struct Frustum
{
	// ������
	Frustum() = default;
	Frustum(const std::array<Plane, 6>& InPlanes);

	// ����Լ� 
	FORCEINLINE BoundCheckResult CheckBound(const Vector3& InPoint) const;
	FORCEINLINE BoundCheckResult CheckBound(const Sphere& InSphere) const;

	// ������� 	
	std::array<Plane, 6> Planes; // Y+, Y-, X+, X-, Z+, Z- ������ ����
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