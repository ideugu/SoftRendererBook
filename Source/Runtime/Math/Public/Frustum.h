#pragma once

namespace CK
{

struct Frustum
{
	// 생성자
	Frustum() = default;
	Frustum(const std::array<Plane, 6>& InPlanes);
	Frustum(const Matrix4x4& InMatrix);

	// 멤버함수 
	FORCEINLINE bool IsOutside(const Vector3& InPoint) const;

	// 멤버변수 	
	std::array<Plane, 6> Planes; // Yup, Ydown, Xright, Xleft, Zup, Zdown의 순으로 저장
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


}