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
	FORCEINLINE bool IsOutside(const Box& InBox) const;
	FORCEINLINE bool IsIntersect(const Sphere& InSphere) const;
	FORCEINLINE bool IsIntersect(const Box& InBox) const;

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

FORCEINLINE bool Frustum::IsOutside(const Box& InBox) const
{
	for (const auto& p : Planes)
	{
		Vector3 pVertex = InBox.Min, nVertex = InBox.Max;
		if (p.Normal.X >= 0.f) { pVertex.X = InBox.Max.X; nVertex.X = InBox.Min.X; }
		if (p.Normal.Y >= 0.f) { pVertex.Y = InBox.Max.Y; nVertex.Y = InBox.Min.Y; }
		if (p.Normal.Z >= 0.f) { pVertex.Z = InBox.Max.Z; nVertex.Z = InBox.Min.Z; }

		if (p.Distance(nVertex) > 0.f)
		{
			return true;
		}
	}

	return false;
}

FORCEINLINE bool Frustum::IsIntersect(const Box& InBox) const
{
	for (const auto& p : Planes)
	{
		Vector3 pVertex = InBox.Min, nVertex = InBox.Max;
		if (p.Normal.X >= 0.f) { pVertex.X = InBox.Max.X; nVertex.X = InBox.Min.X; }
		if (p.Normal.Y >= 0.f) { pVertex.Y = InBox.Max.Y; nVertex.Y = InBox.Min.Y; }
		if (p.Normal.Z >= 0.f) { pVertex.Z = InBox.Max.Z; nVertex.Z = InBox.Min.Z; }

		if (p.Distance(nVertex) <= 0.f && p.Distance(pVertex) >= 0.f)
		{
			return true;
		}
	}

	return false;
}

}