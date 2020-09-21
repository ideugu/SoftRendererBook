
#include "Precompiled.h"
#include <random>

using namespace CK::DD;

const std::string GameEngine::QuadMeshKey("SM_Quad");
const std::string GameEngine::PlayerKey("Player");
const std::string GameEngine::SteveTextureKey("Steve.png");

void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	// ȭ�� ũ���� ����
	_ScreenSize = InScreenSize;
}

bool GameEngine::Init()
{
	// ȭ�� ũ�Ⱑ �ùٷ� �����Ǿ� �ִ��� Ȯ��
	if (_ScreenSize.HasZero())
	{
		return false;
	}

	// �Է� �ý��� üũ
	if (!CheckInputSystem())
	{
		return false;
	}

	// ���ӿ� ����� ���ҽ� �ε�
	if (!LoadResources())
	{
		return false;
	}

	// ���� �� �ε�
	if (!LoadScene())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadResources()
{
	// �޽� ������ �ε�
	Mesh quadMesh;

	float squareHalfSize = 0.5f;
	int vertexCount = 4;
	int triangleCount = 2;
	int indexCount = triangleCount * 3;

	quadMesh._Vertices = {
		Vector2(-squareHalfSize, -squareHalfSize),
		Vector2(-squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, -squareHalfSize)
	};

	quadMesh._UVs = {
		Vector2(0.125f, 0.75f),
		Vector2(0.125f, 0.875f),
		Vector2(0.25f, 0.875f),
		Vector2(0.25f, 0.75f)
	};

	quadMesh._Indices = {
		0, 2, 1, 0, 3, 2
	};

	_Meshes.insert({ GameEngine::QuadMeshKey , quadMesh });

	// �ؽ��� �ε�
	_MainTexture = Texture(SteveTextureKey);
	if (!GetMainTexture().IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	static float playerScale = 20.f;
	static float squareScale = 10.f;

	// �÷��̾� ���� ������Ʈ ����
	GameObject player(GameEngine::PlayerKey);
	player.SetMesh(GameEngine::QuadMeshKey);
	player.GetTransform().SetScale(Vector2::One * playerScale);
	player.SetColor(LinearColor::Red);
	InsertGameObject(player);

	// ���� �õ�� �����ϰ� ����
	std::mt19937 generator(0);
	std::uniform_real_distribution<float> dist(-1000.f, 1000.f);

	// 100���� ��� ���� ������Ʈ ����
	for (int i = 0; i < 100; ++i)
	{
		char name[64];
		std::snprintf(name, sizeof(name), "GameObject%d", i);
		GameObject newGo(name);
		newGo.GetTransform().SetPosition(Vector2(dist(generator), dist(generator)));
		newGo.GetTransform().SetScale(Vector2::One * squareScale);
		newGo.SetMesh(GameEngine::QuadMeshKey);
		newGo.SetColor(LinearColor::Blue);
		if (!InsertGameObject(std::move(newGo)))
		{
			// ���� �̸� �ߺ��� �߻��ϸ� �ε� ���.
			return false;
		}
	}

	return true;
}


// �����ϸ鼭 �����ϱ�
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
	auto it = std::lower_bound(_Scene.begin(), _Scene.end(), InName);
	if (it != _Scene.end())
	{
		return (*it);
	}

	return const_cast<GameObject&>(GameObject::NotFound);
}
