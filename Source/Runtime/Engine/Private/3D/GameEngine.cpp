
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

const std::string GameEngine::CubeMeshKey("SM_Cube");
const std::string GameEngine::PlayerKey("Player");

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

	if (!LoadScene(InViewportSize))
	{
		return false;
	}

	_ViewportSize = InViewportSize;
	return true;
}

bool GameEngine::LoadResources()
{
	auto cubeMesh = std::make_unique<Mesh>();
	static const float cubeHalfSize = 0.5f;

	cubeMesh->_Vertices = {
		// Right 
		Vector3(1.f, -1.f, 1.f) * cubeHalfSize,
		Vector3(1.f, 1.f, 1.f) * cubeHalfSize,
		Vector3(1.f, 1.f, -1.f) * cubeHalfSize,
		Vector3(1.f, -1.f, -1.f) * cubeHalfSize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * cubeHalfSize,
		Vector3(-1.f, 1.f, 1.f) * cubeHalfSize,
		Vector3(1.f, 1.f, 1.f) * cubeHalfSize,
		Vector3(1.f, -1.f, 1.f) * cubeHalfSize,
		// Back
		Vector3(1.f, -1.f, -1.f) * cubeHalfSize,
		Vector3(1.f, 1.f, -1.f) * cubeHalfSize,
		Vector3(-1.f, 1.f, -1.f) * cubeHalfSize,
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize,
		// Left
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize,
		Vector3(-1.f, 1.f, -1.f) * cubeHalfSize,
		Vector3(-1.f, 1.f, 1.f) * cubeHalfSize,
		Vector3(-1.f, -1.f, 1.f) * cubeHalfSize,
		// Top
		Vector3(1.f, 1.f, 1.f) * cubeHalfSize,
		Vector3(-1.f, 1.f, 1.f) * cubeHalfSize,
		Vector3(-1.f, 1.f, -1.f) * cubeHalfSize,
		Vector3(1.f, 1.f, -1.f) * cubeHalfSize,
		// Bottom
		Vector3(-1.f, -1.f, 1.f) * cubeHalfSize,
		Vector3(1.f, -1.f, 1.f) * cubeHalfSize,
		Vector3(1.f, -1.f, -1.f) * cubeHalfSize,
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize
	};

	cubeMesh->_Indices = {
			0, 2, 1, 0, 3, 2,
			4, 6, 5, 4, 7, 6,
			8, 10, 9, 8, 11, 10,
			12, 14, 13, 12, 15, 14,
			16, 18, 17, 16, 19, 18,
			20, 22, 21, 20, 23, 22
	};

	_Meshes.insert({ GameEngine::CubeMeshKey , std::move(cubeMesh) });

	return true;
}

bool GameEngine::LoadScene(const ScreenPoint& InScreenSize)
{
	static float cubeScale = 100.f;

	// 플레이어 설정
	auto player = std::make_unique<GameObject>(GameEngine::PlayerKey);
	player->SetMesh(GameEngine::CubeMeshKey);
	player->GetTransform().SetScale(Vector3::One * cubeScale);
	player->SetColor(LinearColor::Blue);
	_GameObjects.push_back(std::move(player));

	// 카메라 설정
	_Camera = std::make_unique<Camera>();
	_Camera->GetTransform().SetPosition(Vector3(500.f, 500.f, -500.f));
	_Camera->SetLookAtRotation(Vector3::Zero);

	return true;
}
