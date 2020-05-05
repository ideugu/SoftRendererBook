#pragma once

class Mesh
{
public:
	Mesh() = default;
	~Mesh();

public:
	void SetMesh(Vector2* InVertexBuffer, int InVertexCount, int* InIndexBuffer, int InIndexCount);
	int GetVertexCount() const { return VertexCount; }
	int GetIndexCount() const { return IndexCount; }
	const Vector2* GetVerticesPtr() const { return VerticesPtr; }
	const int* GetIndicesPtr() const { return IndicesPtr; }
	void Release();

private:
	int VertexCount = 0;
	int IndexCount = 0;

	Vector2* VerticesPtr = nullptr;
	int* IndicesPtr = nullptr;
};

