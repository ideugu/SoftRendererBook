
#include "Precompiled.h"

void Mesh2D::CalculateBounds()
{
	CircleBound = Circle(_Vertices.data(), _Vertices.size());
}
