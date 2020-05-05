#pragma once

class GameObject2D
{
public:
	GameObject2D() = default;
	GameObject2D(const Mesh* InMeshPtr) : _MeshPtr(InMeshPtr) { }

public:
	Transform2D& GetTransform() { return _Transform; }
	const Mesh* GetMesh() { return _MeshPtr; }
	bool HasMesh() { return (_MeshPtr != nullptr); }

private:
	Transform2D _Transform;
	const Mesh* _MeshPtr = nullptr;
};

