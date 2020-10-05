
#include "Precompiled.h"
using namespace CK::DD;

GameObject GameObject::Invalid(Math::InvalidHashName);

void GameObject::SetMesh(const std::string& InMeshKey)
{
	_MeshKey = InMeshKey;
}

const std::string& GameObject::GetMeshKey() const
{
	return _MeshKey;
}
