
#include "Precompiled.h"
using namespace CK;

float Plane::Distance(const Vector3& InPoint) const
{
	return Normal.Dot(InPoint) + D;
}

void Plane::Normalize()
{
	if (Math::EqualsInTolerance(Normal.SizeSquared(), 1.f))
	{
		return;
	}

	float prevSize = Normal.Size();
	Normal /= prevSize;
	D /= prevSize;
}
