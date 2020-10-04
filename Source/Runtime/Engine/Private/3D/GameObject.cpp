
#include "Precompiled.h"
using namespace CK::DDD;

const GameObject GameObject::NotFound(Math::DefaultHashName);

void GameObject::SetMesh(const std::size_t& InMeshKey)
{
	_MeshKey = InMeshKey;
}

std::size_t GameObject::GetMeshKey() const
{
	return _MeshKey;
}
