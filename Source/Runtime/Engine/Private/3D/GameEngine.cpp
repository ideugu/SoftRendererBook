
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

const std::size_t GameEngine::CubeMeshKey = std::hash<std::string>()("SM_Cube");

const std::string GameEngine::Sun("Sun");
const std::string GameEngine::Earth("Earth");
const std::string GameEngine::Moon("Moon");
const std::string GameEngine::CameraRig("CameraRig");

const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::string GameEngine::SteveTexturePath("Steve.png");

struct GameObjectCompare
{
	bool operator()(const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
};


void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	// 화면 크기의 설정
	_ScreenSize = InScreenSize;
	_MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init()
{
	// 화면 크기가 올바로 설정되어 있는지 확인
	if (_ScreenSize.HasZero())
	{
		return false;
	}

	if (!_InputManager.IsInputSystemReady())
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
	static const float cubeHalfSize = 0.5f;

	// 큐브 메시 정보
	static const int vertexCount = 24;
	static const int triangleCount = 12;
	static const int indexCount = triangleCount * 3;

	Mesh cubeMesh;
	cubeMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize, Vector3(-1.f, -1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, -1.f) * cubeHalfSize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, 1.f) * cubeHalfSize, Vector3(1.f, 1.f, 1.f) * cubeHalfSize, Vector3(1.f, -1.f, 1.f) * cubeHalfSize,
		// Back
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize, Vector3(-1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, -1.f) * cubeHalfSize,
		// Left
		Vector3(1.f, -1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, 1.f) * cubeHalfSize, Vector3(1.f, 1.f, 1.f) * cubeHalfSize, Vector3(1.f, 1.f, -1.f) * cubeHalfSize,
		// Top
		Vector3(-1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, 1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, 1.f) * cubeHalfSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, 1.f) * cubeHalfSize, Vector3(-1.f, -1.f, 1.f) * cubeHalfSize
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

	cubeMesh.CalculateBounds();
	AddMesh(GameEngine::CubeMeshKey, cubeMesh);

	// 텍스쳐 로딩
	Texture diffuseTexture(SteveTexturePath);
	if(!diffuseTexture.IsIntialized())
	{
		return false;
	}
	AddTexture(GameEngine::DiffuseTexture, diffuseTexture);

	return true;
}

bool GameEngine::LoadScene()
{
	static const float sunScale = 100.f;
	static const float earthScale = 40.f;
	static const float moonScale = 30.f;
	static const Vector3 earthOffset(500.f, 0.0f, 0.f);
	static const Vector3 moonOffset(400.f, 0.0f, 0.f);

	// 태양 ( 최상단 )
	GameObject& sun = CreateNewGameObject(GameEngine::Sun);
	sun.SetMesh(GameEngine::CubeMeshKey);
	sun.GetTransformNode().SetWorldScale(Vector3::One * sunScale);

	// 지구
	GameObject& earth = CreateNewGameObject(GameEngine::Earth);
	earth.SetMesh(GameEngine::CubeMeshKey);
	earth.GetTransformNode().SetWorldScale(Vector3::One * earthScale);
	earth.GetTransformNode().SetWorldPosition(earthOffset);
	earth.SetParent(sun);

	// 달
	GameObject& moon = CreateNewGameObject(GameEngine::Moon);
	moon.SetMesh(GameEngine::CubeMeshKey);
	moon.GetTransformNode().SetWorldScale(Vector3::One * moonScale);
	moon.GetTransformNode().SetWorldPosition(moonOffset);
	moon.SetParent(earth);

	// 카메라 릭
	GameObject& cameraRig = CreateNewGameObject(GameEngine::CameraRig);

	// 카메라 설정
	_MainCamera.GetTransformNode().SetWorldPosition(Vector3(700.f, 700.f, -700.f));
	_MainCamera.SetParent(cameraRig);

	return true;
}

bool GameEngine::AddMesh(const std::size_t& InKey, const Mesh& InMesh)
{
	auto meshPtr = std::make_unique<Mesh>(InMesh);
	return _Meshes.insert({ InKey, std::move(meshPtr) }).second;
}

bool GameEngine::AddTexture(const std::size_t& InKey, const Texture& InTexture)
{
	auto texturePtr = std::make_unique<Texture>(InTexture);
	return _Textures.insert({ InKey, std::move(texturePtr) }).second;
}

GameObject& GameEngine::CreateNewGameObject(const std::string& InName)
{
	std::size_t inHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());

	auto newGameObject = std::make_unique<GameObject>(InName);
	if (it != _Scene.end())
	{
		std::size_t targetHash = (*it)->GetHash();
		if (targetHash == inHash)
		{
			// 중복된 키 발생. 무시.
			return const_cast<GameObject&>(GameObject::Invalid);
		}
		else if (targetHash < inHash)
		{
			_Scene.insert(it + 1, std::move(newGameObject));
		}
		else
		{
			_Scene.insert(it, std::move(newGameObject));
		}
	}
	else
	{
		_Scene.push_back(std::move(newGameObject));
	}

	return GetGameObject(InName);
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash, GameObjectCompare());

	if (it != _Scene.end())
	{
		return *(*it).get();
	}

	return const_cast<GameObject&>(GameObject::Invalid);
}
