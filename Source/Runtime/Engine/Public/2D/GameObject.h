#pragma once

namespace CK
{
namespace DD
{

class GameObject
{
public:
	GameObject() = default;
	GameObject(const Mesh* InMeshPtr) : _MeshPtr(InMeshPtr) { }

public:
	Transform& GetTransform() { return _Transform; }
	const Mesh* GetMesh() { return _MeshPtr; }
	bool HasMesh() { return (_MeshPtr != nullptr); }

private:
	Transform _Transform;
	const Mesh* _MeshPtr = nullptr;
};

}
}