
#include "Precompiled.h"
#include "Mesh.h"

Mesh::~Mesh()
{
	Release();
}

void Mesh::SetMesh(Vector2* InVertexBuffer, int InVertexCount, int* InIndexBuffer, int InIndexCount)
{
	VertexCount = InVertexCount;
	IndexCount = InIndexCount;

	VerticesPtr = new Vector2[InVertexCount];
	IndicesPtr = new int[InIndexCount];

	for (int vi = 0; vi < InVertexCount; vi++)
	{
		VerticesPtr[vi] = InVertexBuffer[vi];
	}

	for (int i = 0; i < InIndexCount; i++)
	{
		IndicesPtr[i] = InIndexBuffer[i];
	}
}

void Mesh::Release()
{
	if (nullptr != VerticesPtr)
	{
		delete[] VerticesPtr;
		VerticesPtr = nullptr;
		VertexCount = 0;
	}

	if (nullptr != IndicesPtr)
	{
		delete[] IndicesPtr;
		IndicesPtr = nullptr;
		IndexCount = 0;
	}
}
