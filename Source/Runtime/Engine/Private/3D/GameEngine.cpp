
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

const std::size_t GameEngine::QuadMeshKey = std::hash<std::string>()("SM_Quad");
const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::string GameEngine::QuadKey("Player");
const std::string GameEngine::SteveTexturePath("Steve.png");

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
	// 큐브 메시
	Mesh quadMesh;
	static const float halfSize = 0.5f;

	quadMesh._Vertices = {
		Vector3(-1.f, -1.f, 1.f)* halfSize, Vector3(-1.f, 1.f, 1.f)* halfSize, Vector3(1.f, 1.f, 1.f)* halfSize, Vector3(1.f, -1.f, 1.f)* halfSize
	};

	quadMesh._Indices = {
		0, 2, 1, 0, 3, 2
	};

	quadMesh._UVs = {
		Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f
	};

	// 본 정보 설정
	quadMesh.SetMeshType(MeshType::Skinned);
	quadMesh._Bones = {
		{"left", Bone("left", Transform(Vector3(-1.f, 0.f, 1.f) * halfSize))},
		{"right", Bone("right", Transform(Vector3(1.f, 0.f, 1.f) * halfSize))}
	};
	quadMesh._ConnectedBones = { 1, 1, 1, 1 };
	quadMesh._Weights = {
		{ {"left"}, {1.f} },
		{ {"left"}, {1.f} },
		{ {"right"}, {1.f} },
		{ {"right"}, {1.f} }
	};

	quadMesh.CalculateBounds();
	AddMesh(GameEngine::QuadMeshKey, quadMesh);

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
	// 플레이어 설정
	static float quadScale = 100.f;
	GameObject player(GameEngine::QuadKey);
	player.SetMesh(GameEngine::QuadMeshKey);
	player.GetTransform().SetPosition(Vector3::Zero);
	player.GetTransform().SetScale(Vector3::One * quadScale);
	player.GetTransform().SetRotation(Rotator(180.f, 0.f, 0.f));
	player.SetColor(LinearColor::Blue);
	AddGameObject(std::move(player));

	// 카메라 설정
	_MainCamera.GetTransform().SetPosition(Vector3(0.f, 0.f, -500.f));
	_MainCamera.SetLookAtRotation(Vector3::Zero);

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

// 정렬하면서 삽입하기
bool GameEngine::AddGameObject(const GameObject& InGameObject)
{
	auto goPtr = std::make_unique<GameObject>(InGameObject);

	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), InGameObject, 
		[](const std::unique_ptr<GameObject>& lhs, const GameObject& rhs) 
		{
			return lhs->GetHash() < rhs.GetHash(); 
		}
	);

	if (it == _Scene.end())
	{
		_Scene.push_back(std::move(goPtr));
		return true;
	}

	std::size_t inHash = InGameObject.GetHash();
	std::size_t targetHash = (*it)->GetHash();

	if (targetHash == inHash)
	{
		// 중복된 키 발생. 무시.
		return false;
	}
	else if (targetHash < inHash)
	{
		_Scene.insert(it + 1, std::move(goPtr));
	}
	else
	{
		_Scene.insert(it, std::move(goPtr));
	}

	return true;
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash,
		[](const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
	);

	if (it != _Scene.end())
	{
		return *(*it).get();
	}

	return const_cast<GameObject&>(GameObject::NotFound);
}
