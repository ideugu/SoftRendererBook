
#include "Precompiled.h"
#include <random>

const std::string GameEngine::QuadMeshKey("SM_Quad");
const std::string GameEngine::PlayerKey("Player");

bool GameEngine::Init(const ScreenPoint& InScreenSize)
{
	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.SpacePressed)
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	if (!LoadScene(InScreenSize))
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadResources()
{
	auto quadMesh = std::make_unique<Mesh2D>();

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

	return true;
}

bool GameEngine::LoadScene(const ScreenPoint& InScreenSize)
{
	static float squareScale = 10.f;

	// �÷��̾� ����
	auto player = std::make_unique<GameObject2D>(GameEngine::PlayerKey);
	player->SetMesh(GameEngine::QuadMeshKey);
	player->GetTransform().SetScale(Vector2::One * squareScale);
	player->SetColor(LinearColor::Blue);
	_GameObjects.push_back(std::move(player));

	// ī�޶� ����
	_Camera = std::make_unique<Camera2D>();
	// ī�޶� ���� ������ �ǵ������� �۰� ����
	_Camera->SetCameraViewSize(InScreenSize);
	_Camera->SetCameraCircleBound(250.f);

	// ������ ��� ����
	std::mt19937 generator(0);
	std::uniform_real_distribution<float> dist(-1500.f, 1500.f);

	// 100���� ��� ���� ������Ʈ ����
	for (int i = 0; i < 300; ++i)
	{
		char name[64];
		std::snprintf(name, sizeof(name), "GameObject%d", i);
		auto newGo = std::make_unique<GameObject2D>(name);
		newGo->GetTransform().SetPosition(Vector2(dist(generator), dist(generator)));
		newGo->GetTransform().SetScale(Vector2::One * squareScale);
		newGo->SetMesh(GameEngine::QuadMeshKey);
		_GameObjects.push_back(std::move(newGo));
	}

	return true;
}
