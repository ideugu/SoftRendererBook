
#include "Precompiled.h"
using namespace CK::DDD;

void GameObject::SetMesh(const std::string& InMeshKey)
{
	_MeshKey = InMeshKey;
}

const std::string& GameObject::GetMeshKey() const
{
	return _MeshKey;
}
