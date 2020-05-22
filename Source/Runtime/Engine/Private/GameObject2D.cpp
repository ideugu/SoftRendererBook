
#include "Precompiled.h"

void GameObject2D::SetMesh(const std::string& InMeshKey)
{
	_MeshKey = InMeshKey;
}

const std::string& GameObject2D::GetMeshKey() const
{
	return _MeshKey;
}
