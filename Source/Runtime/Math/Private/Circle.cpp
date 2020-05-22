
#include "Precompiled.h"
#include "Circle.h"

Circle::Circle(const Vector2* InVertices, size_t InCount)
{
	if (InCount == 0)
	{
		return;
	}

	Vector2 sum;
	for (int vi = 0; vi < InCount; ++vi)
	{
		sum += InVertices[vi];
	}

	Center = sum / (float)InCount;

	float distanceSquared = 0.f;
	for (int vi = 0; vi < InCount; ++vi)
	{
		float currentDistanceSquared = (Center - InVertices[vi]).SizeSquared();
		if (currentDistanceSquared > distanceSquared)
		{
			distanceSquared = currentDistanceSquared;
		}
	}

	Radius = sqrtf(distanceSquared);
}
