
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

const std::string GameEngine::CubeMeshKey("SM_Cube");
const std::string GameEngine::PlayerKey("Player");
const std::string GameEngine::SteveTextureKey("Steve.png");

bool GameEngine::Init(const ScreenPoint& InViewportSize)
{
	// 화면 크기의 설정
	_ViewportSize = InViewportSize;

	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.GetZAxis || !_InputManager.GetWAxis || !_InputManager.SpacePressed)
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
	Mesh cubeMesh;
	static const float cubeHalfSize = 0.5f;

	cubeMesh._Vertices = {
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

	cubeMesh._Indices = {
			0, 1, 2, 0, 2, 3, // Right
			4, 6, 5, 4, 7, 6, // Front
			8, 9, 10, 8, 10, 11, // Back
			12, 14, 13, 12, 15, 14, // Left
			16, 18, 17, 16, 19, 18, // Top
			20, 21, 22, 20, 22, 23  // Bottom
	};

	cubeMesh._UVs = {
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

	// 텍스쳐 로딩
	_MainTexture = Texture(SteveTextureKey);
	if (!GetMainTexture().IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	static float cubeScale = 100.f;

	// 플레이어 설정
	GameObject player(GameEngine::PlayerKey);
	player.SetMesh(GameEngine::CubeMeshKey);
	player.GetTransform().SetPosition(Vector3::Zero);
	player.GetTransform().SetScale(Vector3::One * cubeScale);
	player.GetTransform().SetRotation(Rotator(0.f, 0.f, 0.f));
	player.SetColor(LinearColor::Black);
	InsertGameObject(std::move(player));

	// 카메라 설정
	_MainCamera.GetTransform().SetPosition(Vector3(0.f, 200.f, 300.f));
	_MainCamera.SetLookAtRotation(player.GetTransformConst().GetPosition());

	return true;
}

// 정렬하면서 삽입하기
bool GameEngine::InsertGameObject(GameObject& InGameObject)
{
	auto it = std::lower_bound(_Scene.begin(), _Scene.end(), InGameObject);
	if (it == _Scene.end())
	{
		_Scene.push_back(std::move(InGameObject));
		return true;
	}

	std::size_t inHash = InGameObject.GetHash();
	std::size_t targetHash = (*it).GetHash();

	if (targetHash == inHash)
	{
		// 중복된 키 발생. 무시.
		return false;
	}
	else if (targetHash < inHash)
	{
		_Scene.insert(it + 1, std::move(InGameObject));
	}
	else
	{
		_Scene.insert(it, std::move(InGameObject));
	}

	return true;
}

GameObject& GameEngine::FindGameObject(const std::string& InName)
{
	auto it = std::lower_bound(_Scene.begin(), _Scene.end(), InName);
	if (it != _Scene.end())
	{
		return (*it);
	}

	return const_cast<GameObject&>(GameObject::NotFound);
}
