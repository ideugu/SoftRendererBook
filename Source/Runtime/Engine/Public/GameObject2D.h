#pragma once

class GameObject2D
{
public:
	GameObject2D() = default;
	GameObject2D(const Mesh2D* InMeshPtr) : _MeshPtr(InMeshPtr) { }

public:
	Transform2D& GetTransform() { return _Transform; }
	const Mesh2D* GetMesh() { return _MeshPtr; }
	bool HasMesh() { return (_MeshPtr != nullptr); }

protected:
	Transform2D _Transform;
	const Mesh2D* _MeshPtr = nullptr;
};

