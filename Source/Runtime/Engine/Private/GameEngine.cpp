
#include "Precompiled.h"

bool GameEngine::Init(const ScreenPoint& InViewportSize)
{
	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.SpacePressed)
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	if (!LoadScene())
	{
		return false;
	}

	_ViewportSize = InViewportSize;
	return true;
}

bool GameEngine::LoadResources()
{
	_QuadMesh = std::make_unique<DD::Mesh>();

	constexpr float squareHalfSize = 0.5f;
	constexpr int vertexCount = 4;
	constexpr int triangleCount = 2;
	constexpr int indexCount = triangleCount * 3;

	_QuadMesh->_Vertices = {
		Vector2(-squareHalfSize, -squareHalfSize),
		Vector2(-squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, -squareHalfSize)
	};

	_QuadMesh->_Indices = {
		0, 2, 1, 0, 3, 2
	};

	return true;
}

bool GameEngine::LoadScene()
{
	_Player = std::make_unique<DD::GameObject>(_QuadMesh.get());
	return true;
}
