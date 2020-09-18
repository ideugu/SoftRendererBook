
#include "Precompiled.h"
#include <random>

using namespace CK::DD;

const std::string GameEngine::QuadMeshKey("SM_Quad");
const std::string GameEngine::PlayerKey("Player");
const std::string GameEngine::SteveTextureKey("Steve.png");

bool GameEngine::Init(const ScreenPoint& InViewportSize)
{
	// ȭ�� ũ���� ����
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
	// �޽� ������ �ε�
	auto quadMesh = std::make_unique<Mesh>();

	float squareHalfSize = 0.5f;
	int vertexCount = 4;
	int triangleCount = 2;
	int indexCount = triangleCount * 3;

	quadMesh->_Vertices = {
		Vector2(-squareHalfSize, -squareHalfSize),
		Vector2(-squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, -squareHalfSize)
	};

	quadMesh->_Indices = {
		0, 2, 1, 0, 3, 2
	};

	_Meshes.insert({ GameEngine::QuadMeshKey , std::move(quadMesh) });

	// �ؽ��� �ε�
	_MainTexture = std::make_unique<Texture>(SteveTextureKey);
	if (!GetMainTexture().IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	static float squareScale = 10.f;

	// �÷��̾� ���� ������Ʈ ����
	auto player = std::make_unique<GameObject>(GameEngine::PlayerKey);
	player->SetMesh(GameEngine::QuadMeshKey);
	player->GetTransform().SetScale(Vector2::One * squareScale);
	player->SetColor(LinearColor::Blue);
	InsertGameObject(std::move(player));

	// ���� �õ�� �����ϰ� ����
	std::mt19937 generator(0);
	std::uniform_real_distribution<float> dist(-1000.f, 1000.f);

	// 100���� ��� ���� ������Ʈ ����
	for (int i = 0; i < 100; ++i)
	{
		char name[64];
		std::snprintf(name, sizeof(name), "GameObject%d", i);
		auto newGo = std::make_unique<GameObject>(name);
		newGo->GetTransform().SetPosition(Vector2(dist(generator), dist(generator)));
		newGo->GetTransform().SetScale(Vector2::One * squareScale);
		newGo->SetMesh(GameEngine::QuadMeshKey);
		newGo->SetColor(LinearColor::Red);
		if (!InsertGameObject(std::move(newGo)))
		{
			// ���� �̸� �ߺ��� �߻��ϸ� �ε� ���.
			return false;
		}
	}

	return true;
}


// �����ϸ鼭 �����ϱ�
bool GameEngine::InsertGameObject(std::unique_ptr<GameObject> InGameObject)
{
	auto it = std::lower_bound(_Scene.begin(), _Scene.end(), InGameObject->GetName(), OrderByHash());
	if (it == _Scene.end())
	{
		_Scene.push_back(std::move(InGameObject));
		return true;
	}

	std::size_t inHash = InGameObject->GetHash();
	std::size_t targetHash = (*it->get()).GetHash();

	if (targetHash == inHash)
	{
		// �ߺ��� Ű �߻�. ����.
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
	return *std::lower_bound(_Scene.begin(), _Scene.end(), InName, OrderByHash())->get();
}
