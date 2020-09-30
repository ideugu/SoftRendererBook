#pragma once

namespace CK
{

struct Frustum
{
	// ������
	Frustum() = default;
	Frustum(const std::array<Plane, 6>& InPlanes);
	Frustum(const Matrix4x4& InMatrix);

	// ����Լ� 
	FORCEINLINE bool IsOutside(const Vector3& InPoint) const;

	// ������� 	
	std::array<Plane, 6> Planes; // Yup, Ydown, Xright, Xleft, Zup, Zdown�� ������ ����
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