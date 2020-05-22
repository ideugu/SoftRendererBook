
#include "Precompiled.h"
#include "Rectangle.h"

Rectangle::Rectangle(const Vector2* InVertices, size_t InCount)
{
	for (int vi = 0; vi < InCount; ++vi)
	{
		*this += InVertices[vi];
	}
}
