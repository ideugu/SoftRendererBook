
#include "Precompiled.h"
using namespace CK;

void Plane::Normalize()
{
	if (Math::EqualsInTolerance(Normal.SizeSquared(), 1.f))
	{
		return;
	}

	float prevSize = Normal.Size();
	Normal /= prevSize;
	Distance /= prevSize;
}
