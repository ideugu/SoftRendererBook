
#include "Precompiled.h"

void Mesh2D::CalculateBounds()
{
	CircleBounds = Circle(_Vertices.data(), _Vertices.size());
}
