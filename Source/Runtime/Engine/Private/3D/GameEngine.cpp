
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

const std::string GameEngine::CubeMeshKey("SM_Cube");
const std::string GameEngine::PlayerKey("Player");
const std::string GameEngine::SteveTextureKey("Steve");

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
		Vector3(-1.f, -1.f, -1.f)* cubeHalfSize, Vector3(-1.f, -1.f, 1.f)* cubeHalfSize, Vector3(-1.f, 1.f, 1.f)* cubeHalfSize, Vector3(-1.f, 1.f, -1.f)* cubeHalfSize,
		// Front
		Vector3(-1.f, -1.f, 1.f)* cubeHalfSize, Vector3(-1.f, 1.f, 1.f)* cubeHalfSize, Vector3(1.f, 1.f, 1.f)* cubeHalfSize, Vector3(1.f, -1.f, 1.f)* cubeHalfSize,
		// Back
		Vector3(-1.f, -1.f, -1.f)* cubeHalfSize, Vector3(-1.f, 1.f, -1.f)* cubeHalfSize, Vector3(1.f, 1.f, -1.f)* cubeHalfSize, Vector3(1.f, -1.f, -1.f)* cubeHalfSize,
		// Left
		Vector3(1.f, -1.f, -1.f)* cubeHalfSize, Vector3(1.f, -1.f, 1.f)* cubeHalfSize, Vector3(1.f, 1.f, 1.f)* cubeHalfSize, Vector3(1.f, 1.f, -1.f)* cubeHalfSize,
		// Top
		Vector3(-1.f, 1.f, -1.f)* cubeHalfSize, Vector3(1.f, 1.f, -1.f)* cubeHalfSize, Vector3(1.f, 1.f, 1.f)* cubeHalfSize, Vector3(-1.f, 1.f, 1.f)* cubeHalfSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f)* cubeHalfSize, Vector3(1.f, -1.f, -1.f)* cubeHalfSize, Vector3(1.f, -1.f, 1.f)* cubeHalfSize, Vector3(-1.f, -1.f, 1.f)* cubeHalfSize
	};

	cubeMesh->_Indices = {
			0, 1, 2, 0, 2, 3, // Right
			4, 6, 5, 4, 7, 6, // Front
			8, 9, 10, 8, 10, 11, // Back
			12, 14, 13, 12, 15, 14, // Left
			16, 18, 17, 16, 19, 18, // Top
			20, 21, 22, 20, 22, 23  // Bottom
	};

	cubeMesh->_UVs = {
		// Right
		Vector2(0.f, 48.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(0.f, 56.f) / 64.f,
		// Front
		Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f,
		// Back
		Vector2(32.f, 48.f) / 64.f, Vector2(32.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(24.f, 48.f) / 64.f,
		// Left
		Vector2(24.f, 48.f) / 64.f, Vector2(16.f, 48.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f,
		// Top
		Vector2(8.f, 64.f) / 64.f, Vector2(16.f, 64.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(8.f, 56.f) / 64.f,
		// Bottom
		Vector2(16.f, 64.f) / 64.f, Vector2(24.f, 64.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f
	};

	_Meshes.insert({ GameEngine::CubeMeshKey , std::move(cubeMesh) });

	auto steveTexture = std::make_unique<Texture>("Steve.png");
	_Textures.insert({ GameEngine::SteveTextureKey, std::move(steveTexture) });

	return true;
}

bool GameEngine::LoadScene(const ScreenPoint& InScreenSize)
{
	static float cubeScale = 100.f;

	// 플레이어 설정
	auto player = std::make_unique<GameObject>(GameEngine::PlayerKey);
	player->SetMesh(GameEngine::CubeMeshKey);
	player->GetTransform().SetScale(Vector3::One * cubeScale);
	player->GetTransform().SetRotation(Rotator(180.f, 0.f, 0.f));
	player->SetColor(LinearColor::Blue);
	_GameObjects.push_back(std::move(player));

	// 카메라 설정
	_Camera = std::make_unique<Camera>();
	_Camera->GetTransform().SetPosition(Vector3(300.f, 300.f, -300.f));
	_Camera->SetLookAtRotation(Vector3::Zero);

	return true;
}
