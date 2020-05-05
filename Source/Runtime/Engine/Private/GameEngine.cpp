
#include "Precompiled.h"
#include "GameEngine.h"

bool GameEngine::Init()
{
	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.SpacePressed)
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadResources()
{
	_MeshPtr = std::make_unique<Mesh>();

	static const float squareHalfSize = 0.5f;
	static const int vertexCount = 4;
	static const int triangleCount = 2;
	static const int indexCount = triangleCount * 3;

	_MeshPtr->_Vertices = {
		Vector2(-squareHalfSize, -squareHalfSize),
		Vector2(-squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, -squareHalfSize)
	};

	_MeshPtr->_Indices = {
		0, 2, 1, 0, 3, 2
	};

	return true;
}
