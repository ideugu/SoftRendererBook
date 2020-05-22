
#include "Precompiled.h"

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

	if (!LoadScene())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadResources()
{
	_QuadMesh = std::make_unique<Mesh2D>();

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
	static float squareScale = 100.f;

	_Player = std::make_unique<GameObject2D>(_QuadMesh.get());
	_Player->GetTransform().SetScale(Vector2::One * squareScale);

	_Camera = std::make_unique<Camera2D>();
	return true;
}
